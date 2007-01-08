/*
  spaceable-grob.cc -- implement Spaceable_grob

  source file of the GNU LilyPond music typesetter

  (c) 2000--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "spaceable-grob.hh"

#include <cstdio>

#include "warn.hh"
#include "spring.hh"
#include "pointer-group-interface.hh"
#include "grob.hh"
#include "paper-column.hh"
#include "international.hh"

SCM
Spaceable_grob::get_minimum_distances (Grob *me)
{
  return me->get_object ("minimum-distances");
}

/*todo: merge code of spring & rod?
 */
void
Spaceable_grob::add_rod (Grob *me, Grob *p, Real d)
{
  //  printf ("rod %lf\n", d);
  if (d < 0)
    return;

  if (isinf (d))
    programming_error ("infinite rod");

  SCM mins = get_minimum_distances (me);
  SCM newdist = scm_from_double (d);
  for (SCM s = mins; scm_is_pair (s); s = scm_cdr (s))
    {
      SCM dist = scm_car (s);
      if (scm_car (dist) == p->self_scm ())
	{
	  scm_set_cdr_x (dist, scm_max (scm_cdr (dist),
					newdist));
	  return;
	}
    }

  if (Paper_column::get_rank (p) < Paper_column::get_rank (me))
    programming_error ("Adding reverse rod");

  mins = scm_cons (scm_cons (p->self_scm (), newdist), mins);
  me->set_object ("minimum-distances", mins);
}

void
Spaceable_grob::add_spring (Grob *me, Grob *other,
			    Real distance, Real inverse_strength)
{
  if (distance <= 0.0 || inverse_strength < 0.0)
    {
      programming_error ("adding reverse spring, setting to unit");
      distance = 1.0;
      inverse_strength = 1.0;
    }

  if (isinf (distance) || isnan (distance)
      || isnan (inverse_strength))
    {
      /* strength == INF is possible. It means fixed distance.  */
      programming_error ("insane distance found");
      distance = 1.0;
      inverse_strength = 1.0;
    }

#ifndef NDEBUG
  SCM mins = me->get_object ("ideal-distances");
  for (SCM s = mins; scm_is_pair (s); s = scm_cdr (s))
    {
      Spring_smob *sp = unsmob_spring (scm_car (s));
      if (sp->other_ == other)
	{
	  programming_error ("already have that spring");
	  return;
	}
    }
#endif

  Spring_smob spring;
  spring.inverse_strength_ = inverse_strength;
  spring.distance_ = distance;
  spring.other_ = other;

  SCM ideal = me->get_object ("ideal-distances");
  ideal = scm_cons (spring.smobbed_copy (), ideal);
  me->set_object ("ideal-distances", ideal);
}

void
Spaceable_grob::get_spring (Grob *this_col, Grob *next_col, Real *dist, Real *inv_strength)
{
  Spring_smob *spring = 0;

  for (SCM s = this_col->get_object ("ideal-distances");
       !spring && scm_is_pair (s);
       s = scm_cdr (s))
    {
      Spring_smob *sp = unsmob_spring (scm_car (s));

      if (sp && sp->other_ == next_col)
	spring = sp;
    }

  if (!spring)
    programming_error (_f ("No spring between column %d and next one",
			   Paper_column::get_rank (this_col)));

  *dist = (spring) ? spring->distance_ : 5.0;
  *inv_strength = (spring) ? spring->inverse_strength_ : 1.0;
}



ADD_INTERFACE (Spaceable_grob,
	       "A layout object that takes part in the spacing problem. ",
	       

	       /* properties */
	       "allow-loose-spacing "
	       "ideal-distances "
	       "keep-inside-line "
	       "left-neighbors "
	       "measure-length "
	       "minimum-distances "
	       "right-neighbors "
	       "spacing-wishes "

	       );

