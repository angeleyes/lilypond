/*
  beam.cc -- implement Beam

  source file of the GNU LilyPond music typesetter

  (c)  1997--1998, 1998 Han-Wen Nienhuys <hanwen@stack.nl>
    Jan Nieuwenhuizen <jan@digicash.com>

*/


/*
  [TODO]
    * centre beam symbol
    * less hairy code
    * redo grouping 
    * (future) knee: ([\stem 1; c8 \stem -1; c8]
 */

#include <math.h>

#include "p-col.hh"
#include "varray.hh"
#include "proto.hh"
#include "dimen.hh"
#include "beam.hh"
#include "abbreviation-beam.hh"
#include "misc.hh"
#include "debug.hh"
#include "atom.hh"
#include "molecule.hh"
#include "leastsquares.hh"
#include "stem.hh"
#include "paper-def.hh"
#include "lookup.hh"
#include "grouping.hh"
#include "stem-info.hh"
#include "main.hh"  // experimental features


IMPLEMENT_IS_TYPE_B1 (Beam, Spanner);

Beam::Beam ()
{
  slope_f_ = 0;
  left_y_ = 0;
  damping_i_ = 1;
  quantisation_ = NORMAL;
  multiple_i_ = 0;
}

void
Beam::add (Stem*s)
{
  stems_.push (s);
  s->add_dependency (this);
  s->beam_l_ = this;

  if (!spanned_drul_[LEFT])
    set_bounds (LEFT,s);
  else
    set_bounds (RIGHT,s);
}

Molecule*
Beam::brew_molecule_p () const
{
  Molecule *mol_p = new Molecule;

  Real internote_f = paper ()->internote_f ();

  Real x0 = stems_[0]->hpos_f ();
  for (int j=0; j <stems_.size (); j++)
    {
      Stem *i = stems_[j];
      Stem * prev = (j > 0)? stems_[j-1] : 0;
      Stem * next = (j < stems_.size ()-1) ? stems_[j+1] :0;

      Molecule sb = stem_beams (i, next, prev);
      Real  x = i->hpos_f ()-x0;
      sb.translate (Offset (x, (x * slope_f_ + left_y_) * internote_f));
      mol_p->add (sb);
    }
  mol_p->translate_axis (x0 
    - spanned_drul_[LEFT]->absolute_coordinate (X_AXIS), X_AXIS);

  return mol_p;
}

Offset
Beam::center () const
{
  Real w= (paper ()->note_width () + width ().length ())/2.0;
  return Offset (w, (left_y_ + w* slope_f_)*paper ()->internote_f ());
}

void
Beam::do_pre_processing ()
{
  if (!dir_)
    set_default_dir ();
}

void
Beam::do_print () const
{
#ifndef NPRINT
  DOUT << "slope_f_ " <<slope_f_ << "left ypos " << left_y_;
  Spanner::do_print ();
#endif
}

void
Beam::do_post_processing ()
{
  if (stems_.size () < 2)
    {
      warning (_ ("Beam with less than 2 stems"));
      transparent_b_ = true;
      return ;
    }
  solve_slope ();
  set_stemlens ();
}

void
Beam::do_substitute_dependent (Score_elem*o,Score_elem*n)
{
  if (o->is_type_b (Stem::static_name ()))
      stems_.substitute ((Stem*)o->item (),  n? (Stem*) n->item ():0);
}

Interval
Beam::do_width () const
{
  return Interval (stems_[0]->hpos_f (),
		   stems_.top ()->hpos_f ());
}

void
Beam::set_default_dir ()
{
  Drul_array<int> total;
  total[UP]  = total[DOWN] = 0;
  Drul_array<int> count; 
  count[UP]  = count[DOWN] = 0;
  Direction d = DOWN;
  
  for (int i=0; i <stems_.size (); i++)
    do {
      Stem *s = stems_[i];
      int current = s->dir_ 
	? (1 + d * s->dir_)/2
	: s->get_center_distance ((Direction)-d);

      if (current)
	{
	  total[d] += current;
	  count[d] ++;
	}

    } while (flip(&d) != DOWN);
  
   do {
    if (!total[d])
      count[d] = 1;
  } while (flip(&d) != DOWN);
  
  /* 

     [Ross] states that the majority of the notes dictates the
     direction (and not the mean of "center distance")
  */
  dir_ = (total[UP] > total[DOWN]) ? UP : DOWN;

  for (int i=0; i <stems_.size (); i++)
    {
      Stem *sl = stems_[i];
      sl->dir_ = dir_;
    }
}

/*
  See Documentation/tex/fonts.doc
 */
void
Beam::solve_slope ()
{
  /*
    should use minimum energy formulation (cf linespacing)
  */

  assert (multiple_i_);
  Array<Stem_info> sinfo;
  for (int j=0; j <stems_.size (); j++)
    {
      Stem *i = stems_[j];

      i->mult_i_ = multiple_i_;
      i->set_default_extents ();
      if (i->invisible_b ())
	continue;

      Stem_info info (i);
      sinfo.push (info);
    }
  if (! sinfo.size ())
    slope_f_ = left_y_ = 0;
  else if (sinfo.size () == 1)
    {
      slope_f_ = 0;
      left_y_ = sinfo[0].idealy_f_;
    }
  else
    {

      Real leftx = sinfo[0].x_;
      Least_squares l;
      for (int i=0; i < sinfo.size (); i++)
	{
	  sinfo[i].x_ -= leftx;
	  l.input.push (Offset (sinfo[i].x_, sinfo[i].idealy_f_));
	}

      l.minimise (slope_f_, left_y_);
    }

  Real dy = 0.0;
  for (int i=0; i < sinfo.size (); i++)
    {
      Real y = sinfo[i].x_ * slope_f_ + left_y_;
      Real my = sinfo[i].miny_f_;

      if (my - y > dy)
	dy = my -y;
    }
  left_y_ += dy;
  left_y_ *= dir_;

  slope_f_ *= dir_;

  /*
    This neat trick is by Werner Lemberg, damped = tanh (slope_f_) corresponds
    with some tables in [Wanske]
    */
  if (damping_i_)
    slope_f_ = 0.6 * tanh (slope_f_) / damping_i_;

  quantise_dy ();

  Real sl = slope_f_ * paper ()->internote_f ();
  paper ()->lookup_l ()->beam (sl, 20 PT, 1 PT);
  slope_f_ = sl / paper ()->internote_f ();
}

void
Beam::quantise_dy ()
{
  /*
    [Ross] (simplification of)
    Try to set slope_f_ complying with y-span of:
      - zero
      - beam_f / 2 + staffline_f / 2
      - beam_f + staffline_f
    + n * interline
    */

  if (quantisation_ <= NONE)
    return;

  Real interline_f = paper ()->interline_f ();
  Real internote_f = interline_f / 2;
  Real staffline_f = paper ()->rule_thickness ();
  Real beam_f = paper ()->beam_thickness_f ();

  Real dx_f = stems_.top ()->hpos_f () - stems_[0]->hpos_f ();

  // dim(y) = internote; so slope = (y/internote)/x
  Real dy_f = dx_f * abs (slope_f_ * internote_f);
  
  Real quanty_f = 0.0;

  /* UGR.   ICE in 2.8.1; bugreport filed. */
  Array<Real> allowed_fraction (3);
  allowed_fraction[0] = 0;
  allowed_fraction[1] = (beam_f / 2 + staffline_f / 2);
  allowed_fraction[2] = (beam_f + staffline_f);


    Interval iv = quantise_iv (allowed_fraction, interline_f, dy_f);
    quanty_f = (dy_f - iv.min () <= iv.max () - dy_f)
      ? iv.min ()
      : iv.max ();


  slope_f_ = (quanty_f / dx_f) / internote_f * sign (slope_f_);
}

static int test_pos = 0;


/*
  
  Prevent interference from stafflines and beams.  See Documentation/tex/fonts.doc
  
 */
void
Beam::quantise_left_y (bool extend_b)
{
   /*
    we only need to quantise the start of the beam as dy is quantised too
   if extend_b then stems must *not* get shorter
   */

  if (quantisation_ <= NONE)
    return;

  /*
    ----------------------------------------------------------
                                                   ########
	                                ########
                             ########
    --------------########------------------------------------
       ########

       hang       straddle   sit        inter      hang
   */

  Real interline_f = paper ()->interline_f ();
  Real internote_f = paper ()->internote_f ();
  Real staffline_f = paper ()->rule_thickness ();
  Real beam_f = paper ()->beam_thickness_f ();

  /*
    [TODO]
    it would be nice to have all allowed positions in a runtime matrix:
    (multiplicity, minimum_beam_dy, maximum_beam_dy)
   */

  Real straddle = 0;
  Real sit = beam_f / 2 - staffline_f / 2;
  Real inter = interline_f / 2;
  Real hang = interline_f - beam_f / 2 + staffline_f / 2;

  /*
    Put all allowed positions into an array.
    Whether a position is allowed or not depends on 
    strictness of quantisation, multiplicity and direction.

    For simplicity, we'll assume dir = UP and correct if 
    dir = DOWN afterwards.
   */

  // dim(left_y_) = internote
  Real dy_f = dir_ * left_y_ * internote_f;

  Real beamdx_f = stems_.top ()->hpos_f () - stems_[0]->hpos_f ();
  Real beamdy_f = beamdx_f * slope_f_ * internote_f;

  Array<Real> allowed_position;
  if (quantisation_ != TEST)
    {
      if (quantisation_ <= NORMAL) 
	{
	  if ((multiple_i_ <= 2) || (abs (beamdy_f) >= staffline_f / 2))
	    allowed_position.push (straddle);
	  if ((multiple_i_ <= 1) || (abs (beamdy_f) >= staffline_f / 2))
	    allowed_position.push (sit);
	  allowed_position.push (hang);
	}
      else
        // TODO: check and fix TRADITIONAL
	{
	  if ((multiple_i_ <= 2) || (abs (beamdy_f) >= staffline_f / 2))
	    allowed_position.push (straddle);
	  if ((multiple_i_ <= 1) && (beamdy_f <= staffline_f / 2))
	    allowed_position.push (sit);
	  if (beamdy_f >= -staffline_f / 2)
	    allowed_position.push (hang);
	}
    }
  else
    {
      if (test_pos == 0)
        {
	allowed_position.push (hang);
	cout << "hang" << hang << endl;
	}
      else if (test_pos==1)
        {
	allowed_position.push (straddle);
	cout << "straddle" << straddle << endl;
	}
      else if (test_pos==2)
        {
	allowed_position.push (sit);
	cout << "sit" << sit << endl;
	}
      else if (test_pos==3)
        {
	allowed_position.push (inter);
	cout << "inter" << inter << endl;
	}
    }

#if 0
  // this currently never happens
  Real q = (dy_f / interline_f - dy_i) * interline_f;
  if ((quantisation_ < NORMAL) && (q < interline_f / 3 - beam_f / 2))
    allowed_position.push (inter);
#endif

  Interval iv = quantise_iv (allowed_position, interline_f, dy_f);

  Real quanty_f = dy_f - iv.min () <= iv.max () - dy_f ? iv.min () : iv.max ();
  if (extend_b)
    quanty_f = iv.max ();

  // dim(left_y_) = internote
  left_y_ = dir_ * quanty_f / internote_f;
}

void
Beam::set_stemlens ()
{
  Real staffline_f = paper ()->rule_thickness ();

  Real x0 = stems_[0]->hpos_f ();
  Real dy = 0;
  // ugh, rounding problems! (enge floots)
  Real epsilon = staffline_f / 8;
  do
    { 
      left_y_ += dy * dir_;
      quantise_left_y (dy);
      dy = 0;
      for (int i=0; i < stems_.size (); i++)
	{
	  Stem *s = stems_[i];
	  if (s->transparent_b_)
	    continue;

	  Real x = s->hpos_f () - x0;
	  s->set_stemend (left_y_ + slope_f_ * x);
	  Real y = s->stem_end_f () * dir_;
	  Stem_info info (s);
	  if (y < info.miny_f_)
	    dy = dy >? info.miny_f_ - y;
	}
    } while (abs (dy) > epsilon);

  test_pos++;
  test_pos %= 4;
}

/*
 FIXME
 ugh.  this is broken and should be rewritten.
  - [c8. c32 c32]
 */
void
Beam::set_grouping (Rhythmic_grouping def, Rhythmic_grouping cur)
{
  def.OK ();
  cur.OK ();
  assert (cur.children.size () == stems_.size ());

  cur.split (def);

  Array<int> b;
  {
    Array<int> flags;
    for (int j=0; j <stems_.size (); j++)
      {
	Stem *s = stems_[j];

	int f = s->flag_i_ - 2;
	assert (f>0);
	flags.push (f);
      }
    int fi =0;
    b= cur.generate_beams (flags, fi);
    b.insert (0,0);
    b.push (0);
    assert (stems_.size () == b.size ()/2);
  }

  for (int j=0, i=0; i < b.size () && j <stems_.size (); i+= 2, j++)
    {
      Stem *s = stems_[j];
      s->beams_left_i_ = b[i];
      s->beams_right_i_ = b[i+1];
      multiple_i_ = multiple_i_ >? (b[i] >? b[i+1]);
    }
}

/*
  beams to go with one stem.
  */
Molecule
Beam::stem_beams (Stem *here, Stem *next, Stem *prev) const
{
  assert (!next || next->hpos_f () > here->hpos_f ());
  assert (!prev || prev->hpos_f () < here->hpos_f ());

  Real staffline_f = paper ()->rule_thickness ();
  Real interbeam_f = paper ()->interbeam_f (multiple_i_);
  Real internote_f = paper ()->internote_f (); 
  Real beam_f = paper ()->beam_thickness_f ();

  Real dy = interbeam_f;
  Real stemdx = staffline_f;
  Real sl = slope_f_* internote_f;
  paper ()->lookup_l ()->beam (sl, 20 PT, 1 PT);

  Molecule leftbeams;
  Molecule rightbeams;

  /* half beams extending to the left. */
  if (prev)
    {
      int lhalfs= lhalfs = here->beams_left_i_ - prev->beams_right_i_ ;
      int lwholebeams= here->beams_left_i_ <? prev->beams_right_i_ ;
      Real w = (here->hpos_f () - prev->hpos_f ())/4 <? paper ()->note_width ();;
      Atom a;
      if (lhalfs)		// generates warnings if not
	a =  paper ()->lookup_l ()->beam (sl, w, beam_f);
      a.translate (Offset (-w, -w * sl));
      for (int j = 0; j  < lhalfs; j++)
	{
	  Atom b (a);
	  b.translate_axis (-dir_ * dy * (lwholebeams+j), Y_AXIS);
	  leftbeams.add (b);
	}
    }

  if (next)
    {
      int rhalfs = here->beams_right_i_ - next->beams_left_i_;
      int rwholebeams = here->beams_right_i_ <? next->beams_left_i_;

      Real w = next->hpos_f () - here->hpos_f ();
      Atom a = paper ()->lookup_l ()->beam (sl, w + stemdx, beam_f);
      a.translate_axis( - stemdx/2, X_AXIS);
      int j = 0;
      Real gap_f = 0;
      if (here->beam_gap_i_)
	{
	  int nogap = rwholebeams - here->beam_gap_i_;
	  for (; j  < nogap; j++)
	    {
	      Atom b (a);
	      b.translate_axis (-dir_ * dy * j, Y_AXIS);
	      rightbeams.add (b);
	    }
	  // TODO: notehead widths differ for different types
	  gap_f = paper ()->note_width () / 2;
	  w -= 2 * gap_f;
	  a = paper ()->lookup_l ()->beam (sl, w + stemdx, beam_f);
	}

      for (; j  < rwholebeams; j++)
	{
	  Atom b (a);
	  b.translate (Offset (gap_f, -dir_ * dy * j));
	  rightbeams.add (b);
	}

      w = w/4 <? paper ()->note_width ();
      if (rhalfs)
	a = paper ()->lookup_l ()->beam (sl, w, beam_f);

      for (; j  < rwholebeams + rhalfs; j++)
	{
	  Atom b (a);
	  b.translate_axis (-dir_ * dy * j, Y_AXIS);
	  rightbeams.add (b);
	}

    }
  leftbeams.add (rightbeams);

  /*
    Does beam quanting think  of the asymetry of beams? 
    Refpoint is on bottom of symbol. (FIXTHAT) --hwn.
   */
  return leftbeams;
}

