/*
  breathing_sign.cc -- implement Breathing_sign

  (c)  1999--2003 Michael Krause

  written for the GNU LilyPond music typesetter

  TODO: --> see breathing-sign-engraver.cc

  Extensions for ancient notation (c)  2003 by Juergen Reuter
*/

#include "staff-symbol-referencer.hh"
#include "directional-element-interface.hh"
#include "breathing-sign.hh"
#include "string.hh"
#include "molecule.hh"
#include "paper-def.hh"
#include "lookup.hh"

#include "dimensions.hh"
#include "direction.hh"
#include "text-item.hh"
#include "font-interface.hh"

/*
  TODO: thickness should be a grob property (unit: stafflinethickness)
 rather than hardwired to (staff_space / 6).
 */

  

/*
  UGH : this is full of C&P code. Consolidate!  --hwn
*/

/*
  Gregorian chant divisio minima.  (Actually, this was the original
  breathing sign by Michael. -- jr)
 */
MAKE_SCHEME_CALLBACK (Breathing_sign,divisio_minima,1);
SCM 
Breathing_sign::divisio_minima (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  Real staff_space = Staff_symbol_referencer::staff_space (me);
  Real staff_size;
  Real thickness = me->get_paper ()->get_realvar (ly_symbol2scm ("linethickness"));
  SCM lt =  me->get_grob_property ("thickness");
  if (gh_number_p (lt))
    thickness *= gh_scm2double (lt);
  if (Staff_symbol_referencer::get_staff_symbol (me))
    {
      staff_size = (Staff_symbol_referencer::line_count (me) - 1) * staff_space;
    }
  else
    staff_size = 0.0;

  Real blotdiameter = me->get_paper ()->get_realvar (ly_symbol2scm ("blotdiameter"));

  /*
   * Draw a small vertical line through the uppermost (or, depending
   * on direction, lowermost) staff line.
   */
  Interval xdim (0, thickness);
  Interval ydim (-0.5 * staff_space, +0.5 * staff_space);
  Box b (xdim, ydim);
  Molecule out = Lookup::round_filled_box (b, blotdiameter);
  return out.smobbed_copy ();
}

/*
  Gregorian chant divisio maior.
 */
MAKE_SCHEME_CALLBACK (Breathing_sign,divisio_maior,1);
SCM 
Breathing_sign::divisio_maior (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  Real staff_space = Staff_symbol_referencer::staff_space (me);
  Real staff_size;
  Real thickness = me->get_paper ()->get_realvar (ly_symbol2scm ("linethickness"));
  SCM lt =  me->get_grob_property ("thickness");
  if (gh_number_p (lt))
    thickness *= gh_scm2double (lt);
  if (Staff_symbol_referencer::get_staff_symbol (me))
    {
      staff_size = (Staff_symbol_referencer::line_count (me) - 1) * staff_space;
    }
  else
    staff_size = 0.0;

  Real blotdiameter = me->get_paper ()->get_realvar (ly_symbol2scm ("blotdiameter"));

  /*
   * Draw a vertical line that is vertically centered in the staff
   * (just like a bar).  The height of this line should be a little
   * more than half the size of the staff, such that the endings of
   * the line are in the middle of a staff space.
   */
  int lines = Staff_symbol_referencer::line_count (me);
  int height = lines / 2; // little more than half of staff size
  if ((lines & 1) != (height & 1))
    height++; // ensure endings are centered in staff space

  Interval xdim (0, thickness);
  Interval ydim (-0.5 * height, +0.5 * height);
  Box b (xdim, ydim);
  Molecule out = Lookup::round_filled_box (b, blotdiameter);
  return out.smobbed_copy ();
}

/*
  Gregorian chant divisio maxima.
 */
MAKE_SCHEME_CALLBACK (Breathing_sign,divisio_maxima,1);
SCM 
Breathing_sign::divisio_maxima (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  Real staff_space = Staff_symbol_referencer::staff_space (me);
  Real staff_size;
  Real thickness = me->get_paper ()->get_realvar (ly_symbol2scm ("linethickness"));
  SCM lt =  me->get_grob_property ("thickness");
  if (gh_number_p (lt))
    thickness *= gh_scm2double (lt);
  if (Staff_symbol_referencer::get_staff_symbol (me))
    {
      staff_size = (Staff_symbol_referencer::line_count (me) - 1) * staff_space;
    }
  else
    staff_size = 0.0;

  Real blotdiameter = me->get_paper ()->get_realvar (ly_symbol2scm ("blotdiameter"));

  // like a "|" type bar
  Interval xdim (0, thickness);
  Interval ydim (-0.5 * staff_size, +0.5 * staff_size);
  Box b (xdim, ydim);
  Molecule out = Lookup::round_filled_box (b, blotdiameter);
  return out.smobbed_copy ();
}

/*
  Gregorian chant finalis.
 */
MAKE_SCHEME_CALLBACK (Breathing_sign,finalis,1);
SCM 
Breathing_sign::finalis (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  Real staff_space = Staff_symbol_referencer::staff_space (me);
  Real staff_size;
  Real thickness = me->get_paper ()->get_realvar (ly_symbol2scm ("linethickness"));
  SCM lt =  me->get_grob_property ("thickness");
  if (gh_number_p (lt))
    thickness *= gh_scm2double (lt);
  if (Staff_symbol_referencer::get_staff_symbol (me))
    {
      staff_size = (Staff_symbol_referencer::line_count (me) - 1) * staff_space;
    }
  else
    staff_size = 0.0;

  Real blotdiameter = me->get_paper ()->get_realvar (ly_symbol2scm ("blotdiameter"));

  // like a "||" type bar
  Interval xdim (0, thickness);
  Interval ydim (-0.5 * staff_size, +0.5 * staff_size);
  Box b (xdim, ydim);
  Molecule line1 = Lookup::round_filled_box (b, blotdiameter);
  Molecule line2 (line1);
  line2.translate_axis (0.5 * staff_space, X_AXIS);
  line1.add_molecule (line2);

  return line1.smobbed_copy ();
}

MAKE_SCHEME_CALLBACK (Breathing_sign,offset_callback,2);
SCM
Breathing_sign::offset_callback (SCM element_smob, SCM)
{
  Grob *me = unsmob_grob (element_smob);
  
  Direction d = get_grob_direction (me);
  if (!d)
    {
      d = UP;
      set_grob_direction (me, d);
    }

  Real inter_f = Staff_symbol_referencer::staff_space (me)/2;
  int sz = Staff_symbol_referencer::line_count (me)-1;
  return gh_double2scm (inter_f * sz * d);
}

ADD_INTERFACE(Breathing_sign, "breathing-sign-interface",
	      "A breathing sign.",
	      "direction");
