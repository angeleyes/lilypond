/*
  beam-quanting.cc -- implement Beam quanting functions

  source file of the GNU LilyPond music typesetter

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include <math.h>

#include "warn.hh"
#include "staff-symbol-referencer.hh"
#include "beam.hh"
#include "stem.hh"
#include "output-def.hh"
#include "group-interface.hh"
#include "align-interface.hh"

const int INTER_QUANT_PENALTY = 1000;
const Real SECONDARY_BEAM_DEMERIT = 10.0;
const int STEM_LENGTH_DEMERIT_FACTOR = 5;

/*
  threshold to combat rounding errors.
*/
const Real BEAM_EPS = 1e-3;

// possibly ridiculous, but too short stems just won't do
const int STEM_LENGTH_LIMIT_PENALTY = 5000;
const int DAMPING_DIRECTION_PENALTY = 800;
const int MUSICAL_DIRECTION_FACTOR = 400;
const int IDEAL_SLOPE_FACTOR = 10;
const Real ROUND_TO_ZERO_SLOPE = 0.02;

static Real
shrink_extra_weight (Real x, Real fac)
{
  return fabs (x) * ((x < 0) ? fac : 1.0);
}

struct Quant_score
{
  Real yl;
  Real yr;
  Real demerits;

#if DEBUG_QUANTING
  String score_card_;
#endif
};

/*
  TODO:

  - Make all demerits customisable

  - One sensible check per demerit (what's this --hwn)

  - Add demerits for quants per se, as to forbid a specific quant
  entirely
*/

int
best_quant_score_idx (Array<Quant_score> const &qscores)
{
  Real best = 1e6;
  int best_idx = -1;
  for (int i = qscores.size (); i--;)
    {
      if (qscores[i].demerits < best)
	{
	  best = qscores [i].demerits;
	  best_idx = i;
	}
    }

  if (best_idx < 0)
    {
      programming_error ("Huh? No best beam quant score?");
      best_idx = 0;
    }

  return best_idx;
}

MAKE_SCHEME_CALLBACK (Beam, quanting, 1);
SCM
Beam::quanting (SCM smob)
{
  Grob *me = unsmob_grob (smob);

  SCM s = me->get_property ("positions");
  Real yl = scm_to_double (scm_car (s));
  Real yr = scm_to_double (scm_cdr (s));

  /*
    Calculations are relative to a unit-scaled staff, i.e. the quants are
    divided by the current staff_space.

  */
  Real ss = Staff_symbol_referencer::staff_space (me);
  Real thickness = Beam::get_thickness (me) / ss;
  Real slt = Staff_symbol_referencer::line_thickness (me) / ss;

  Real dy_mus = robust_scm2double (me->get_property ("least-squares-dy"), 0);
  Real straddle = 0.0;
  Real sit = (thickness - slt) / 2;
  Real inter = 0.5;
  Real hang = 1.0 - (thickness - slt) / 2;
  Real quants [] = {straddle, sit, inter, hang };

  int num_quants = int (sizeof (quants) / sizeof (Real));
  Array<Real> quantsl;
  Array<Real> quantsr;

  /*
    going to REGION_SIZE == 2, yields another 0.6 second with
    wtk1-fugue2.


    (result indexes between 70 and 575)  ? --hwn.

  */

  /*
    Do stem computations.  These depend on YL and YR linearly, so we can
    precompute for every stem 2 factors.
  */
  Link_array<Grob> stems
    = extract_grob_array (me, ly_symbol2scm ("stems"));
  Array<Stem_info> stem_infos;
  Array<Real> base_lengths;
  Array<Real> stem_xposns;

  Drul_array<bool> dirs_found (0, 0);
  Grob *common[2];
  for (int a = 2; a--;)
    common[a] = common_refpoint_of_array (stems, me, Axis (a));

  Grob *fvs = first_visible_stem (me);
  Grob *lvs = last_visible_stem (me);
  Real xl = fvs ? fvs->relative_coordinate (common[X_AXIS], X_AXIS) : 0.0;
  Real xr = fvs ? lvs->relative_coordinate (common[X_AXIS], X_AXIS) : 0.0;

  /*
    We store some info to quickly interpolate.

    Sometimes my head is screwed on backwards.  The stemlength are
    AFFINE linear in YL and YR. If YL == YR == 0, then we might have
    stem_y != 0.0, when we're cross staff.

  */
  for (int i = 0; i < stems.size (); i++)
    {
      Grob *s = stems[i];

      Stem_info si (Stem::get_stem_info (s));
      si.scale (1 / ss);
      stem_infos.push (si);
      dirs_found[stem_infos.top ().dir_] = true;

      bool f = to_boolean (s->get_property ("french-beaming"))
	&& s != lvs && s != fvs;

      base_lengths.push (calc_stem_y (me, s, common, xl, xr,
				      Interval (0, 0), f) / ss);
      stem_xposns.push (s->relative_coordinate (common[X_AXIS], X_AXIS));
    }

  bool xstaff = false;
  if (lvs && fvs)
    {
      Grob *commony = fvs->common_refpoint (lvs, Y_AXIS);
      xstaff = Align_interface::has_interface (commony);
    }

  Direction ldir = Direction (stem_infos[0].dir_);
  Direction rdir = Direction (stem_infos.top ().dir_);
  bool is_knee = dirs_found[LEFT] && dirs_found[RIGHT];

  int region_size = REGION_SIZE;
  /*
    Knees are harder, lets try some more possibilities for knees.
  */
  if (is_knee)
    region_size += 2;

  /*
    Asymetry ? should run to <= region_size ?
  */
  for (int i = -region_size; i < region_size; i++)
    for (int j = 0; j < num_quants; j++)
      {
	quantsl.push (i + quants[j] + int (yl));
	quantsr.push (i + quants[j] + int (yr));
      }

  Array<Quant_score> qscores;

  for (int l = 0; l < quantsl.size (); l++)
    for (int r = 0; r < quantsr.size (); r++)
      {
	Quant_score qs;
	qs.yl = quantsl[l];
	qs.yr = quantsr[r];
	qs.demerits = 0.0;

	qscores.push (qs);
      }

  /* This is a longish function, but we don't separate this out into
     neat modular separate subfunctions, as the subfunctions would be
     called for many values of YL, YR. By precomputing various
     parameters outside of the loop, we can save a lot of time. */
  for (int i = qscores.size (); i--;)
    {
      Real d = score_slopes_dy (qscores[i].yl, qscores[i].yr,
				dy_mus, yr- yl,
				xr - xl,
				xstaff);
      qscores[i].demerits += d;

#if DEBUG_QUANTING
      qscores[i].score_card_ += to_string ("S%.2f", d);
#endif
    }

  Real rad = Staff_symbol_referencer::staff_radius (me);
  Drul_array<int> edge_beam_counts
    (Stem::beam_multiplicity (stems[0]).length () + 1,
     Stem::beam_multiplicity (stems.top ()).length () + 1);

  Real beam_translation = get_beam_translation (me) / ss;

  Real reasonable_score = (is_knee) ? 200000 : 100;
  for (int i = qscores.size (); i--;)
    if (qscores[i].demerits < reasonable_score)
      {
	Real d = score_forbidden_quants (qscores[i].yl, qscores[i].yr,
					 rad, slt, thickness, beam_translation,
					 edge_beam_counts, ldir, rdir);
	qscores[i].demerits += d;

#if DEBUG_QUANTING
	qscores[i].score_card_ += to_string (" F %.2f", d);
#endif
      }

  for (int i = qscores.size (); i--;)
    if (qscores[i].demerits < reasonable_score)
      {
	Real d = score_stem_lengths (stems, stem_infos,
				     base_lengths, stem_xposns,
				     xl, xr,
				     is_knee,
				     qscores[i].yl, qscores[i].yr);
	qscores[i].demerits += d;

#if DEBUG_QUANTING
	qscores[i].score_card_ += to_string (" L %.2f", d);
#endif
      }

  int best_idx = best_quant_score_idx (qscores);

#if DEBUG_QUANTING
  SCM inspect_quants = me->get_property ("inspect-quants");
  if (to_boolean (me->get_layout ()->lookup_variable (ly_symbol2scm ("debug-beam-quanting")))
      && scm_is_pair (inspect_quants))
    {
      Drul_array<Real> ins = ly_scm2interval (inspect_quants);

      int i = 0;

      Real mindist = 1e6;
      for (; i < qscores.size (); i++)
	{
	  Real d = fabs (qscores[i].yl- ins[LEFT]) + fabs (qscores[i].yr - ins[RIGHT]);
	  if (d < mindist)
	    {
	      best_idx = i;
	      mindist = d;
	    }
	}
      if (mindist > 1e5)
	programming_error ("Could not find quant.");
    }
#endif

  me->set_property ("positions",
		    ly_interval2scm (Drul_array<Real> (qscores[best_idx].yl,
						       qscores[best_idx].yr)));
#if DEBUG_QUANTING
  if (to_boolean (me->get_layout ()->lookup_variable (ly_symbol2scm ("debug-beam-quanting"))))
    {
      qscores[best_idx].score_card_ += to_string ("i%d", best_idx);

      // debug quanting
      me->set_property ("quant-score",
			scm_makfrom0str (qscores[best_idx].score_card_.to_str0 ()));
    }
#endif

  return SCM_UNSPECIFIED;
}

Real
Beam::score_stem_lengths (Link_array<Grob> const &stems,
			  Array<Stem_info> const &stem_infos,
			  Array<Real> const &base_stem_ys,
			  Array<Real> const &stem_xs,
			  Real xl, Real xr,
			  bool knee,
			  Real yl, Real yr)
{
  Real limit_penalty = STEM_LENGTH_LIMIT_PENALTY;
  Drul_array<Real> score (0, 0);
  Drul_array<int> count (0, 0);

  for (int i = 0; i < stems.size (); i++)
    {
      Grob *s = stems[i];
      if (Stem::is_invisible (s))
	continue;

      Real x = stem_xs[i];
      Real dx = xr - xl;
      Real beam_y = dx ? yr * (x - xl) / dx + yl * (xr - x) / dx : (yr + yl) / 2;
      Real current_y = beam_y + base_stem_ys[i];
      Real length_pen = STEM_LENGTH_DEMERIT_FACTOR;

      Stem_info info = stem_infos[i];
      Direction d = info.dir_;

      score[d] += limit_penalty * (0 >? (d * (info.shortest_y_ - current_y)));

      Real ideal_diff = d * (current_y - info.ideal_y_);
      Real ideal_score = shrink_extra_weight (ideal_diff, 1.5);

      /* We introduce a power, to make the scoring strictly
         convex. Otherwise a symmetric knee beam (up/down/up/down)
         does not have an optimum in the middle. */
      if (knee)
	ideal_score = pow (ideal_score, 1.1);

      score[d] += length_pen * ideal_score;

      count[d]++;
    }

  Direction d = DOWN;
  do
    {
      score[d] /= (count[d] >? 1);
    }
  while (flip (&d) != DOWN);

  return score[LEFT] + score[RIGHT];
}

Real
Beam::score_slopes_dy (Real yl, Real yr,
		       Real dy_mus, Real dy_damp,
		       Real dx,
		       bool xstaff)
{
  Real dy = yr - yl;
  Real dem = 0.0;

  /*
    DAMPING_DIRECTION_PENALTY is a very harsh measure, while for
    complex beaming patterns, horizontal is often a good choice.

    TODO: find a way to incorporate the complexity of the beam in this
    penalty.
  */
  if (fabs (dy / dx) > ROUND_TO_ZERO_SLOPE
      && sign (dy_damp) != sign (dy))
    {
      dem += DAMPING_DIRECTION_PENALTY;
    }

  dem += MUSICAL_DIRECTION_FACTOR *(0 >? (fabs (dy) - fabs (dy_mus)));

  Real slope_penalty = IDEAL_SLOPE_FACTOR;

  /* Xstaff beams tend to use extreme slopes to get short stems. We
     put in a penalty here. */
  if (xstaff)
    slope_penalty *= 10;

  /* Huh, why would a too steep beam be better than a too flat one ? */
  dem += shrink_extra_weight (fabs (dy_damp) - fabs (dy), 1.5)
    * slope_penalty;

  return dem;
}

static Real
my_modf (Real x)
{
  return x - floor (x);
}

/*
  TODO: The fixed value SECONDARY_BEAM_DEMERIT is probably flawed:
  because for 32nd and 64th beams the forbidden quants are relatively
  more important than stem lengths.
*/
Real
Beam::score_forbidden_quants (Real yl, Real yr,
			      Real radius,
			      Real slt,
			      Real thickness, Real beam_translation,
			      Drul_array<int> beam_counts,
			      Direction ldir, Direction rdir)
{
  Real dy = yr - yl;
  Drul_array<Real> y (yl, yr);
  Drul_array<Direction> dirs (ldir, rdir);

  Real extra_demerit = SECONDARY_BEAM_DEMERIT / (beam_counts[LEFT] >? beam_counts[RIGHT]);

  Direction d = LEFT;
  Real dem = 0.0;

  do
    {
      for (int j = 1; j <= beam_counts[d]; j++)
	{
	  Direction stem_dir = dirs[d];

	  /*
	    The 2.2 factor is to provide a little leniency for
	    borderline cases. If we do 2.0, then the upper outer line
	    will be in the gap of the (2, sit) quant, leading to a
	    false demerit.
	  */
	  Real gap1 = y[d] - stem_dir * ((j - 1) * beam_translation + thickness / 2 - slt / 2.2);
	  Real gap2 = y[d] - stem_dir * (j * beam_translation - thickness / 2 + slt / 2.2);

	  Interval gap;
	  gap.add_point (gap1);
	  gap.add_point (gap2);

	  for (Real k = -radius;
	       k <= radius + BEAM_EPS; k += 1.0)
	    if (gap.contains (k))
	      {
		Real dist = fabs (gap[UP] - k) <? fabs (gap[DOWN] - k);

		/*
		  this parameter is tuned to grace-stem-length.ly
		*/
		Real fixed_demerit = 0.4;

		dem += extra_demerit
		  * (fixed_demerit
		     + (1 - fixed_demerit) * (dist / gap.length ())* 2);
	      }
	}
    }
  while ((flip (&d))!= LEFT);

  if ((beam_counts[LEFT] >? beam_counts[RIGHT]) >= 2)
    {
      Real straddle = 0.0;
      Real sit = (thickness - slt) / 2;
      Real inter = 0.5;
      Real hang = 1.0 - (thickness - slt) / 2;

      Direction d = LEFT;
      do
	{
	  if (beam_counts[d] >= 2
	      && fabs (y[d] - dirs[d] * beam_translation) < radius + inter)
	    {
	      if (dirs[d] == UP && dy <= BEAM_EPS
		  && fabs (my_modf (y[d]) - sit) < BEAM_EPS)
		dem += extra_demerit;

	      if (dirs[d] == DOWN && dy >= BEAM_EPS
		  && fabs (my_modf (y[d]) - hang) < BEAM_EPS)
		dem += extra_demerit;
	    }

	  if (beam_counts[d] >= 3
	      && fabs (y[d] - 2 * dirs[d] * beam_translation) < radius + inter)
	    {
	      if (dirs[d] == UP && dy <= BEAM_EPS
		  && fabs (my_modf (y[d]) - straddle) < BEAM_EPS)
		dem += extra_demerit;

	      if (dirs[d] == DOWN && dy >= BEAM_EPS
		  && fabs (my_modf (y[d]) - straddle) < BEAM_EPS)
		dem += extra_demerit;
	    }
	}
      while (flip (&d) != LEFT);
    }

  return dem;
}

