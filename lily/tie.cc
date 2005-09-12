/*
  tie.cc -- implement Tie

  source file of the GNU LilyPond music typesetter

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "tie.hh"

#include <math.h>

#include "spanner.hh"
#include "lookup.hh"
#include "output-def.hh"
#include "rhythmic-head.hh"
#include "bezier.hh"
#include "paper-column.hh"
#include "warn.hh"
#include "staff-symbol-referencer.hh"
#include "directional-element-interface.hh"
#include "bezier.hh"
#include "stem.hh"
#include "note-head.hh"
#include "tie-column.hh"

int
Tie::compare (Grob *const &s1,
	      Grob *const &s2)
{
  return sign (Tie::get_position (s1) - Tie::get_position (s2));
}

void
Tie::set_head (Grob *me, Direction d, Grob *h)
{
  dynamic_cast<Spanner *> (me)->set_bound (d, h);
  me->add_dependency (h);
}

Grob *
Tie::head (Grob *me, Direction d)
{
  Item *it = dynamic_cast<Spanner*> (me)->get_bound (d);
  if (Note_head::has_interface (it))
    return it;
  else
    return 0;
}

int
Tie::get_column_rank (Grob *me, Direction d)
{
  Spanner *span = dynamic_cast<Spanner *> (me);
  Grob *h = head (me, d);
  if (!h)
    h = span->get_bound (d);

  Grob *col = dynamic_cast<Item *> (h)->get_column ();
  return Paper_column::get_rank (col);
}

int
Tie::get_position (Grob *me)
{
  Direction d = head (me, LEFT) ? LEFT : RIGHT;
  return (int) Staff_symbol_referencer::get_position (head (me, d));
}

/*
  Default:  Put the tie oppositie of the stem [Wanske p231]

  In case of chords: Tie_column takes over

  The direction of the Tie is more complicated (See [Ross] p136 and
  further).

  (what about linebreaks? )
*/
Direction
Tie::get_default_dir (Grob *me)
{
  Item *sl = head (me, LEFT) ? Rhythmic_head::get_stem (head (me, LEFT)) : 0;
  Item *sr = head (me, RIGHT) ? Rhythmic_head::get_stem (head (me, RIGHT)) : 0;
  if (sl && sr)
    {
      if (get_grob_direction (sl) == UP
	  && get_grob_direction (sr) == UP)
	return DOWN;
    }
  else if (sl || sr)
    {
      Item *s = sl ? sl : sr;
      return -get_grob_direction (s);
    }

  return UP;
}

void
Tie::set_direction (Grob *me)
{
  if (!get_grob_direction (me))
    {
      if (Tie_column::has_interface (me->get_parent (Y_AXIS)))
	Tie_column::set_directions (me->get_parent (Y_AXIS));
      else
	set_grob_direction (me, Tie::get_default_dir (me));
    }
}

Interval
Tie::get_default_attachments (Spanner *me, Grob *common, Real gap,
			      int *staff_position,
			      bool *in_between
			      )
{
  Real staff_space = Staff_symbol_referencer::staff_space (me);
  Direction dir = get_grob_direction (me);
  Interval attachments;
  Direction d = LEFT;
  do
    {
      attachments[d]
	= robust_relative_extent (me->get_bound (d),
				  common,
				  X_AXIS)[-d]
	- gap * d;
    }
  while (flip (&d) != LEFT);

  if (attachments.length () < 0.6 * staff_space)
    {
      /*
	Let short ties start over note heads, instead of between.
      */
      Drul_array<bool> allow (true, true);

      Direction d = LEFT;
      do {
	if (Note_head::has_interface (me->get_bound (d)))
	  {
	    Grob *stem = unsmob_grob (me->get_bound (d)->get_object ("stem"));
	    if (get_grob_direction (stem) == dir
		&& -d == dir)
	      allow[d] = false;
	  }
      } while (flip (&d) != LEFT);

      if (allow[LEFT] && allow[RIGHT])
	{
	  *staff_position += dir;
	  do
	    {
	      if (Note_head::has_interface (me->get_bound (d)))
		{
		  Interval extent
		    = robust_relative_extent (me->get_bound (d),
					      common, X_AXIS);

		  attachments[d] = extent.linear_combination (- 0.5 * d);
		  *in_between = false;
		}
	    }
	  while (flip (&d) != LEFT);
	}
    }

  return attachments;
}  

Interval
get_skyline_attachment (Drul_array< Array < Skyline_entry > > const &skylines,
			Real y)
{
  Interval attachments;
  Direction d = LEFT;
  do
    {
      attachments[d] = skyline_height (skylines[d], y, -d);
    }
  while (flip (&d) != LEFT);
  
  return attachments;
}
			
void
Tie::get_configuration (Grob *me_grob, Grob *common,
			Tie_configuration *conf,
			Drul_array< Array < Skyline_entry > > const *skylines,
			Tie_details const &details
			)
{
  Spanner *me = dynamic_cast<Spanner*> (me_grob);
  if (!head (me, LEFT) && !head (me, RIGHT))
    {
      programming_error ("tie without heads");
      me->suicide ();
      return ;
    }

  /*
    UGH. Don't mirror Tie_configuration.
   */
  conf->head_position_ = (int) Tie::get_position (me);
  if (!conf->dir_)
    conf->dir_ = get_grob_direction (me);
  if (!conf->dir_)
    conf->dir_ = get_default_dir (me);
    

  Real staff_space = details.staff_space_;
  bool in_between = true;
  Real gap = robust_scm2double (me->get_property ("x-gap"), 0.2);

  if (conf->attachment_x_.is_empty())
    {
      if (!skylines)
	conf->attachment_x_ = get_default_attachments (me, common, gap,
					       &conf->position_,
					       &in_between);
      else
	{
	  Real y = staff_space * 0.5 * conf->position_;
	  conf->attachment_x_ = get_skyline_attachment (*skylines, y);
	  conf->attachment_x_.widen (-gap);
	}
    }

  Bezier b = slur_shape (conf->attachment_x_.length(),
			 details.height_limit_,
			 details.ratio_);
  b.scale (1, conf->dir_);
  
  Offset middle = b.curve_point (0.5);
  Offset edge = b.curve_point (0.0);

  conf->position_ = int (rint (conf->position_));
  
  Real dy = fabs (middle[Y_AXIS] - edge[Y_AXIS]);
  bool in_space = !(Staff_symbol_referencer::on_staffline (me, (int) conf->position_));
  bool fits_in_space =
    (dy < 0.6 * staff_space);
  
  /*
    Avoid dot
   */
  Grob *left_dot = unsmob_grob (me->get_bound (LEFT)->get_object ("dot"));
  int dot_pos = left_dot
    ? int (Staff_symbol_referencer::get_position (left_dot))
    : 0;
  if (left_dot
      && (conf->position_ == dot_pos
	  || conf->position_ + conf->dir_ == dot_pos))
    {
      conf->position_ += conf->dir_;
      in_space = !in_space;

      if (skylines)
	{
	  Real y = staff_space * 0.5 * conf->position_;
	  conf->attachment_x_ = get_skyline_attachment (*skylines, y);
	  conf->attachment_x_.widen (-gap);
	  Bezier b = slur_shape (conf->attachment_x_.length(),
				 details.height_limit_,
				 details.ratio_);
	  Offset middle = b.curve_point (0.5);
	  Offset edge = b.curve_point (0.0);
	  dy = fabs (middle[Y_AXIS] - edge[Y_AXIS]);
	  fits_in_space =
	    (dy < 0.6 * staff_space);
	}
    }
  
  /*
    Avoid flag.
  */
  Grob *left_stem = unsmob_grob (me->get_bound (LEFT)->get_object ("stem"));
  if (left_stem)
    {
      Stencil flag = Stem::get_translated_flag (left_stem);
      Real y = conf->position_ * staff_space * 0.5;
      if (flag.extent (Y_AXIS).contains (y))
	{
	  conf->position_ += conf->dir_;
	  in_space = !in_space;
	}
    }

  if (in_space != fits_in_space)
    {
      if (in_space)
	{
	  conf->position_ += conf->dir_;
	}
      else
	{
	  in_space = true;
	  conf->position_ += conf->dir_;
	}

      /*
	ugh: code dup.
       */
      if (skylines)
	{
	  Real y = staff_space * 0.5 * conf->position_;
	  conf->attachment_x_ = get_skyline_attachment (*skylines, y);
	  conf->attachment_x_.widen (-gap);
	      
	  Bezier b = slur_shape (conf->attachment_x_.length(),
				 details.height_limit_,
				 details.ratio_);
	  Offset middle = b.curve_point (0.5);
	  Offset edge = b.curve_point (0.0);
	  dy = fabs (middle[Y_AXIS] - edge[Y_AXIS]);
	  fits_in_space =
	    (dy < 0.6 * staff_space);
	}
    }


  /*
    Putting larger in-space ties next to the notes forces
    the edges to be opposite (Y-wise) to the tie direction.
   */
  if (conf->position_ == conf->head_position_
      && in_space
      && Staff_symbol_referencer::staff_radius (me) > fabs (conf->position_) / 2
      && dy > 0.3 * staff_space)
    {
      conf->position_ += 2 * conf->dir_; 
    }

  if (!in_between
      && in_space
      && fabs (conf->position_ - conf->head_position_) <= 1)
    conf->position_ += 2*conf->dir_;
  
  
  if (in_space)
    {
      if ((fabs (conf->position_ - conf->head_position_) <= 1
	   && fabs (dy) < 0.45 * staff_space)
	  || fabs (dy) < 0.6 * staff_space)
	{
	  /*
	    vertically center in space.
	  */
	  conf->center_tie_vertically (details);
	}
      else
	{
	  conf->delta_y_ = 
	    conf->dir_ * staff_space * (- 0.3);
	}
    }
  else
    {
      Real where = 0.5 * conf->dir_;
      
      Real rounding_dy = (where - middle[Y_AXIS]);
      conf->delta_y_ = rounding_dy;

      if (conf->dir_ * (b.curve_point (0.0)[Y_AXIS]
		 + conf->position_ * staff_space * 0.5
		 + conf->delta_y_) <
	  conf->dir_ * conf->head_position_ * 0.5 * staff_space)
	{
	  if (Staff_symbol_referencer::staff_radius (me) >  fabs (conf->head_position_) / 2)
	    conf->position_ +=  2 * conf->dir_;
	  else
	    conf->position_ += conf->dir_;
	}
    }


  if (skylines)
    {
      Real half_space = 0.5 * staff_space;
      Real y = conf->position_ * half_space;
      
      conf->attachment_x_ = get_skyline_attachment (*skylines, y);
      conf->attachment_x_.widen (-gap);
    }
}


void
Tie::set_default_control_points (Grob *me_grob)
{
  Spanner *me = dynamic_cast<Spanner*> (me_grob);
  Grob *common  = me;
  common = me->get_bound (LEFT)->common_refpoint (common, X_AXIS); 
  common = me->get_bound (RIGHT)->common_refpoint (common, X_AXIS); 
  
  Tie_configuration conf;
  if (!get_grob_direction (me))
    set_grob_direction (me, get_default_dir (me));

  int tie_position = (int) Tie::get_position (me);
  conf.position_ = tie_position;
  
  Tie_details details;
  details.init (me);
  get_configuration (me, common, &conf, 0, details);
  set_control_points (me, common, conf, details);
}

void
Tie::set_control_points (Grob *me,
			 Grob *common,
			 Tie_configuration const &conf,
			 Tie_details const &details
			 )
{
  Bezier b = conf.get_bezier (details);
  b.scale (1, conf.dir_);
  b.translate (Offset (conf.attachment_x_[LEFT]
		       - me->relative_coordinate (common, X_AXIS),
		       0.5 * conf.position_ * details.staff_space_
		       + conf.delta_y_
		       ));
  
  SCM controls = SCM_EOL;
  for (int i = 4; i--;)
    {
      if (!b.control_[i].is_sane ())
	programming_error ("Insane offset");
      controls = scm_cons (ly_offset2scm (b.control_[i]), controls);
    }
  me->set_property ("control-points", controls);
}



MAKE_SCHEME_CALLBACK (Tie, print, 1);
SCM
Tie::print (SCM smob)
{
  Grob *me = unsmob_grob (smob);

  if (CENTER == get_grob_direction (me))
    set_direction (me);

  if (!get_grob_direction (me))
    me->programming_error ("Tie direction not set."); 
    
  SCM cp = me->get_property ("control-points");
  if (!scm_is_pair (cp))
    {
      /*
	UGH.  dependency tracking!
       */
      if (Tie_column::has_interface (me->get_parent (Y_AXIS)))
	{
	  Tie_column::set_directions (me->get_parent (Y_AXIS));
	}

      cp = me->get_property ("control-points");
    }
  
  if (!scm_is_pair (cp))
    {
      set_default_control_points (me);
      cp = me->get_property ("control-points");
    }

  if (!scm_is_pair (cp))
    return Stencil ().smobbed_copy ();

  Real staff_thick = Staff_symbol_referencer::line_thickness (me);
  Real base_thick = robust_scm2double (me->get_property ("thickness"), 1);
  Real thick = base_thick * staff_thick;

  Bezier b;
  int i = 0;
  for (SCM s = cp; s != SCM_EOL; s = scm_cdr (s))
    {
      b.control_[i] = ly_scm2offset (scm_car (s));
      i++;
    }

  Stencil a;

  SCM p = me->get_property ("dash-period");
  SCM f = me->get_property ("dash-fraction");
  if (scm_is_number (p) && scm_is_number (f))
    a = Lookup::dashed_slur (b,
			     thick,
			     robust_scm2double (p, 1.0),
			     robust_scm2double (f, 0));
  else
    a = Lookup::slur (b,
		      get_grob_direction (me) * staff_thick,
		      thick);

  return a.smobbed_copy ();
}

ADD_INTERFACE (Tie,
	       "tie-interface",
	       
	       "A tie connecting two noteheads.\n",

	       /* properties */
	       "control-points "
	       "dash-fraction "
	       "dash-period "
	       "details "
	       "direction "
	       "thickness "
	       "x-gap ");
