/*   
  percent-repeat-item.cc --  implement Percent_repeat_item_interface
  
  source file of the GNU LilyPond music typesetter
  
  (c) 2001--2004  Han-Wen Nienhuys <hanwen@cs.uu.nl>
  
 */
#include <math.h>

#include "grob.hh"
#include "lookup.hh"
#include "stencil.hh"
#include "font-interface.hh"
#include "font-metric.hh" 
#include "percent-repeat-item.hh"


Stencil
Percent_repeat_item_interface::brew_slash ( Grob *me)
{
  Real slope = robust_scm2double (me->get_property ("slope"), 1);
  Real wid = 2.0 / slope;

  /*
    todo: check out if in staff-rule thickness normally.
   */
  Real thick = robust_scm2double (me->get_property ("thickness"), 1);
  Stencil m = Lookup::repeat_slash (wid, slope, thick);
  m.translate_axis (-m.extent (Y_AXIS).center (), Y_AXIS);
  return m;
}

/*
  todo: use grob props for dot_neg_kern, slash_neg_kern?
 */
Stencil
Percent_repeat_item_interface::x_percent (Grob *me, int count,
					  Real dot_neg_kern,
					  Real slash_neg_kern)
{
  Stencil m ;
  Stencil s = brew_slash (me);

  for (int i  = count; i--;)
    {
      m.add_at_edge (X_AXIS, RIGHT, s, -slash_neg_kern,0);
    }
  Stencil d1 = Font_interface::get_default_font (me)->find_by_name ("dots-dot");
  Stencil d2  =  d1;
  d1.translate_axis (0.5, Y_AXIS );
  d2.translate_axis (-0.5, Y_AXIS);
  
  m.add_at_edge (X_AXIS, LEFT, d1, -dot_neg_kern,0);
  m.add_at_edge (X_AXIS, RIGHT, d2, -dot_neg_kern,0);

  return m;
}

MAKE_SCHEME_CALLBACK (Percent_repeat_item_interface,double_percent,1);
SCM
Percent_repeat_item_interface::double_percent (SCM grob)
{
  Grob *me = unsmob_grob (grob);
  Stencil m = x_percent (me, 2, 0.75, 1.6);
  m.translate_axis (- m.extent (X_AXIS).center (), X_AXIS);
  return m.smobbed_copy ();
}

MAKE_SCHEME_CALLBACK (Percent_repeat_item_interface,beat_slash,1);
SCM
Percent_repeat_item_interface::beat_slash (SCM grob)
{
  Grob *me = unsmob_grob (grob);
  Stencil m = brew_slash (me);

  return m.smobbed_copy ();
}

ADD_INTERFACE (Percent_repeat_item_interface,"percent-repeat-interface",
  "Repeats that look like percent signs",
  "slope thickness");



