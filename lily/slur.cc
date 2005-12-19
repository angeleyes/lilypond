/*
  slur.cc -- implement external interface for Slur

  source file of the GNU LilyPond music typesetter

  (c) 1996--2005 Han-Wen Nienhuys <hanwen@xs4all.nl>
  Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include "slur.hh"


#include "beam.hh"
#include "bezier.hh"
#include "directional-element-interface.hh"
#include "font-interface.hh"
#include "pointer-group-interface.hh"
#include "lookup.hh"
#include "main.hh"		// DEBUG_SLUR_SCORING
#include "note-column.hh"
#include "output-def.hh"
#include "spanner.hh"
#include "staff-symbol-referencer.hh"
#include "staff-symbol.hh"
#include "stem.hh"
#include "text-interface.hh"
#include "warn.hh"
#include "slur-scoring.hh"

#include "script-interface.hh"



MAKE_SCHEME_CALLBACK(Slur, calc_direction, 1)
SCM
Slur::calc_direction (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  extract_grob_set (me, "note-columns", encompasses);

  if (encompasses.is_empty ())
    {
      me->suicide ();
      return SCM_BOOL_F;
    }

  Direction d = DOWN;
  for (int i = 0; i < encompasses.size (); i++)
    {
      if (Note_column::dir (encompasses[i]) < 0)
	{
	  d = UP;
	  break;
	}
    }
  return scm_from_int (d);
}

MAKE_SCHEME_CALLBACK (Slur, height, 1);
SCM
Slur::height (SCM smob)
{
  Grob *me = unsmob_grob (smob);

  // FIXME uncached
  Stencil *m = me->get_stencil ();
  return m ? ly_interval2scm (m->extent (Y_AXIS)) : ly_interval2scm (Interval ());
}

/*
  Ugh should have dash-length + dash-period
*/
MAKE_SCHEME_CALLBACK (Slur, print, 1);
SCM
Slur::print (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  extract_grob_set (me, "note-columns", encompasses);
  if (encompasses.is_empty ())
    {
      me->suicide ();
      return SCM_EOL;
    }

  Real staff_thick = Staff_symbol_referencer::line_thickness (me);
  Real base_thick = robust_scm2double (me->get_property ("thickness"), 1);
  Real thick = base_thick * staff_thick;
  Bezier one = get_curve (me);
  Stencil a;

  /*
    TODO: replace dashed with generic property.
  */
  SCM p = me->get_property ("dash-period");
  SCM f = me->get_property ("dash-fraction");
  if (scm_is_number (p) && scm_is_number (f))
    a = Lookup::dashed_slur (one, thick, robust_scm2double (p, 1.0),
			     robust_scm2double (f, 0));
  else
    a = Lookup::slur (one,
		      get_grob_direction (me) * staff_thick * 1.0,
		      thick);

#if DEBUG_SLUR_SCORING
  SCM quant_score = me->get_property ("quant-score");

  if (to_boolean (me->layout ()
		  ->lookup_variable (ly_symbol2scm ("debug-slur-scoring")))
      && scm_is_string (quant_score))
    {
      String str;
      SCM properties = Font_interface::text_font_alist_chain (me);

      Stencil tm = *unsmob_stencil (Text_interface::interpret_markup
				    (me->layout ()->self_scm (), properties,
				     quant_score));
      a.add_at_edge (Y_AXIS, get_grob_direction (me), tm, 1.0, 0);
    }
#endif

  return a.smobbed_copy ();
}

Bezier
Slur::get_curve (Grob *me)
{
  Bezier b;
  int i = 0;
  for (SCM s = me->get_property ("control-points"); s != SCM_EOL;
       s = scm_cdr (s))
    b.control_[i++] = ly_scm2offset (scm_car (s));

  return b;
}

void
Slur::add_column (Grob *me, Grob *n)
{
  Pointer_group_interface::add_grob (me, ly_symbol2scm ("note-columns"), n);
  add_bound_item (dynamic_cast<Spanner *> (me), dynamic_cast<Item *> (n));
}

void
Slur::add_extra_encompass (Grob *me, Grob *n)
{
  Pointer_group_interface::add_grob (me, ly_symbol2scm ("encompass-objects"), n);
}


MAKE_SCHEME_CALLBACK (Slur, outside_slur_callback, 2);
SCM
Slur::outside_slur_callback (SCM grob, SCM offset_scm)
{
  Grob *script = unsmob_grob (grob);
  Grob *slur = unsmob_grob (script->get_object ("slur"));

  if (!slur)
    return offset_scm;

  SCM avoid = script->get_property ("avoid-slur");
  if (avoid != ly_symbol2scm ("outside"))
    return scm_from_int (0);
  
  Direction dir = get_grob_direction (script);
  if (dir == CENTER)
    return offset_scm;

  Grob *cx = script->common_refpoint (slur, X_AXIS);
  Grob *cy = script->common_refpoint (slur, Y_AXIS);

  Bezier curve = Slur::get_curve (slur);

  curve.translate (Offset (slur->relative_coordinate (cx, X_AXIS),
			   slur->relative_coordinate (cy, Y_AXIS)));

  Interval yext = robust_relative_extent (script, cy, Y_AXIS);
  Interval xext = robust_relative_extent (script, cx, X_AXIS);

  yext.translate (robust_scm2double (offset_scm, 0));
  
  
  /* FIXME: slur property, script property?  */
  Real slur_padding = robust_scm2double (script->get_property ("slur-padding"),
					 0.0);
  yext.widen (slur_padding);

  Real EPS = 1e-3;
  Interval bezext (curve.control_[0][X_AXIS], curve.control_[3][X_AXIS]);
  bool consider[] = { false, false, false };
  Real ys[] = {0, 0, 0};
  bool do_shift = false;
  
  for (int d = LEFT, k = 0; d <= RIGHT; d++, k++)
    {
      Real x = xext.linear_combination ((Direction) d);
      consider[k] = bezext.contains (x);

      if (consider[k])
	{
	  ys[k]
	    = (fabs (bezext[LEFT] - x) < EPS)
	    ? curve.control_[0][Y_AXIS]
	    : ((fabs (bezext[RIGHT] - x) < EPS)
	       ? curve.control_[3][Y_AXIS]
	       : curve.get_other_coordinate (X_AXIS, x));
	  consider[k] = true;

	  /* Request shift if slur is contained script's Y, or if
	     script is inside slur and avoid == outside.  */
	  if (yext.contains (ys[k])
	      || dir * ys[k] > dir * yext[-dir])
	    do_shift = true;
	}
    }

  Real avoidance_offset = 0.0;
  if (do_shift)
    {
      for (int d = LEFT, k = 0; d <= RIGHT; d++, k++)
	avoidance_offset = dir * (max (dir * avoidance_offset,
			     dir * (ys[k] - yext[-dir] + dir * slur_padding)));
    }

  return scm_from_double (scm_to_double (offset_scm) + avoidance_offset);
}



ADD_INTERFACE (Slur, "slur-interface",
	       
	       "A slur",
	       
	       /* properties */
	       "control-points "
	       "dash-fraction "
	       "dash-period "
	       "direction "
	       "eccentricity "
	       "encompass-objects "
	       "height-limit "
	       "note-columns "
	       "positions "
	       "quant-score "
	       "ratio "
	       "details "
	       "thickness ");

