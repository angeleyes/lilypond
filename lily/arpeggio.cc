/*
  arpeggio.cc -- implement Arpeggio

  source file of the GNU LilyPond music typesetter

  (c) 2000--2005 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include "arpeggio.hh"

#include "output-def.hh"
#include "stem.hh"
#include "staff-symbol-referencer.hh"
#include "staff-symbol.hh"
#include "warn.hh"
#include "font-interface.hh"
#include "lookup.hh"
#include "pointer-group-interface.hh"

MAKE_SCHEME_CALLBACK (Arpeggio, print, 1);
SCM
Arpeggio::print (SCM smob)
{
  Grob *me = unsmob_grob (smob);

  Grob *common = me;

  extract_grob_set (me, "stems", stems);
  for (int i = 0; i < stems.size (); i++)
    {
      Grob *stem = stems[i];
      common = common->common_refpoint (Staff_symbol_referencer::get_staff_symbol (stem),
					Y_AXIS);
    }

  /*
    TODO:

    Using stems here is not very convenient; should store noteheads
    instead, and also put them into the support. Now we will mess up
    in vicinity of a collision.

  */
  Interval heads;
  Real my_y = me->relative_coordinate (common, Y_AXIS);

  for (int i = 0; i < stems.size (); i++)
    {
      Grob *stem = stems[i];
      Grob *ss = Staff_symbol_referencer::get_staff_symbol (stem);
      Interval iv = Stem::head_positions (stem);
      iv *= Staff_symbol::staff_space (ss) / 2.0;

      heads.unite (iv + ss->relative_coordinate (common, Y_AXIS)
		   - my_y);
    }

  if (heads.is_empty () || heads.length () < 0.5)
    {
      programming_error ("no heads for arpeggio found?");
      me->suicide ();
      return SCM_EOL;
    }

  SCM ad = me->get_property ("arpeggio-direction");
  Direction dir = CENTER;
  if (is_direction (ad))
    dir = to_dir (ad);

  Stencil mol;
  Font_metric *fm = Font_interface::get_default_font (me);
  Stencil squiggle = fm->find_by_name ("scripts.arpeggio");

  Stencil arrow;
  if (dir)
    {
      arrow = fm->find_by_name ("scripts.arpeggio.arrow." + to_string (dir));
      heads[dir] -= dir * arrow.extent (Y_AXIS).length ();
    }

  for (Real y = heads[LEFT]; y < heads[RIGHT];
       y += squiggle.extent (Y_AXIS).length ())
    mol.add_at_edge (Y_AXIS, UP, squiggle, 0.0, 0);

  mol.translate_axis (heads[LEFT], Y_AXIS);
  if (dir)
    mol.add_at_edge (Y_AXIS, dir, arrow, 0, 0);

  return mol.smobbed_copy ();
}

/* Draws a vertical bracket to the left of a chord
   Chris Jackson <chris@fluffhouse.org.uk> */

MAKE_SCHEME_CALLBACK (Arpeggio, brew_chord_bracket, 1);
SCM
Arpeggio::brew_chord_bracket (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  Grob *common = me;

  extract_grob_set (me, "stems", stems);
  for (int i = 0; i < stems.size (); i++)
    {
      Grob *stem = stems[i];
      common = common->common_refpoint (Staff_symbol_referencer::get_staff_symbol (stem),
					Y_AXIS);
    }

  Interval heads;
  Real my_y = me->relative_coordinate (common, Y_AXIS);

  for (int i = 0; i < stems.size (); i++)
    {
      Grob *stem = stems[i];
      Grob *ss = Staff_symbol_referencer::get_staff_symbol (stem);
      Interval iv = Stem::head_positions (stem);
      iv *= Staff_symbol::staff_space (ss) / 2.0;
      heads.unite (iv + ss->relative_coordinate (common, Y_AXIS) - my_y);
    }

  Real lt = me->get_layout ()->get_dimension (ly_symbol2scm ("linethickness"));
  Real sp = 1.5 * Staff_symbol_referencer::staff_space (me);
  Real dy = heads.length () + sp;
  Real x = 0.7;

  Stencil mol (Lookup::bracket (Y_AXIS, Interval (0, dy), lt, x, lt));
  mol.translate_axis (heads[LEFT] - sp / 2.0, Y_AXIS);
  return mol.smobbed_copy ();
}

/*
  We have to do a callback, because print () triggers a
  vertical alignment if it is cross-staff.
*/
MAKE_SCHEME_CALLBACK (Arpeggio, width, 1);
SCM
Arpeggio::width (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  Stencil arpeggio = Font_interface::get_default_font (me)->find_by_name ("scripts.arpeggio");

  return ly_interval2scm (arpeggio.extent (X_AXIS));
}

ADD_INTERFACE (Arpeggio, "arpeggio-interface",
	       "Functions and settings for drawing an arpeggio symbol (a wavy line left to noteheads.",

	       /* properties */
	       "arpeggio-direction "
	       "stems "
	       );

