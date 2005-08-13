/*
  mensural-ligature.cc -- implement Mensural_ligature

  source file of the GNU LilyPond music typesetter

  (c) 2002--2005 Juergen Reuter <reuter@ipd.uka.de>,
  Pal Benko <benkop@freestart.hu>
*/

#include <math.h>

#include "mensural-ligature.hh"
#include "item.hh"
#include "font-interface.hh"
#include "lookup.hh"
#include "staff-symbol-referencer.hh"
#include "note-head.hh"
#include "output-def.hh"
#include "warn.hh"

/*
 * TODO: divide this function into mensural and neo-mensural style.
 *
 * TODO: move this function to class Lookup?
 */
Stencil
brew_flexa (Grob *me,
	    Real interval,
	    bool solid,
	    Real width,
	    Real vertical_line_thickness)
{
  Real staff_space = Staff_symbol_referencer::staff_space (me);
  Real slope = (interval / 2.0 * staff_space) / width;

  // Compensate optical illusion regarding vertical position of left
  // and right endings due to slope.
  Real ypos_correction = -0.1 * staff_space * sign (slope);
  Real slope_correction = 0.2 * staff_space * sign (slope);
  Real corrected_slope = slope + slope_correction / width;

  Stencil stencil;
  if (solid) // colorated flexae
    {
      Stencil solid_head
	= Lookup::beam (corrected_slope, width, staff_space, 0.0);
      stencil.add_stencil (solid_head);
    }
  else // outline
    {
      /*
	The thickness of the horizontal lines of the flexa shape
	should be equal to that of the horizontal lines of the
	neomensural brevis note head (see mf/parmesan-heads.mf).
      */
      Real const horizontal_line_thickness = staff_space * 0.35;

      // URGH!  vertical_line_thickness is adjustable (via thickness
      // property), while horizontal_line_thickness is constant.
      // Maybe both should be adjustable independently?

      Real height = staff_space - horizontal_line_thickness;

      Stencil left_edge
	= Lookup::beam (corrected_slope, vertical_line_thickness, height, 0.0);
      stencil.add_stencil (left_edge);

      Stencil right_edge
	= Lookup::beam (corrected_slope, vertical_line_thickness, height, 0.0);
      right_edge.translate_axis (width - vertical_line_thickness, X_AXIS);
      right_edge.translate_axis ((width - vertical_line_thickness) *
				 corrected_slope, Y_AXIS);
      stencil.add_stencil (right_edge);

      Stencil bottom_edge
	= Lookup::beam (corrected_slope, width,
			horizontal_line_thickness, 0.0);
      bottom_edge.translate_axis (-0.5 * height, Y_AXIS);
      stencil.add_stencil (bottom_edge);

      Stencil top_edge
	= Lookup::beam (corrected_slope, width,
			horizontal_line_thickness, 0.0);
      top_edge.translate_axis (+0.5 * height, Y_AXIS);
      stencil.add_stencil (top_edge);
    }
  stencil.translate_axis (ypos_correction, Y_AXIS);
  return stencil;
}

Stencil
internal_brew_primitive (Grob *me)
{
  SCM primitive_scm = me->get_property ("primitive");
  if (primitive_scm == SCM_EOL)
    {
      programming_error ("Mensural_ligature: "
			 "undefined primitive -> ignoring grob");
      return Stencil ();
    }
  int primitive = scm_to_int (primitive_scm);

  Stencil out;
  int delta_pitch = 0;
  Real thickness = 0.0;
  Real width = 0.0;
  Real staff_space = Staff_symbol_referencer::staff_space (me);
  if (primitive & MLP_ANY)
    thickness = robust_scm2double (me->get_property ("thickness"), .14);

  if (primitive & MLP_FLEXA)
    {
      delta_pitch = robust_scm2int (me->get_property ("delta-pitch"),
				    0);
      width
	= robust_scm2double (me->get_property ("flexa-width"), 2.0 * staff_space);
    }
  if (primitive & MLP_SINGLE_HEAD)
    width = robust_scm2double (me->get_property ("head-width"), staff_space);

  switch (primitive & MLP_ANY)
    {
    case MLP_NONE:
      return Stencil ();
    case MLP_LONGA: // mensural brevis head with right cauda
      out = Font_interface::get_default_font (me)->find_by_name
	("noteheads.s-2mensural");
      break;
    case MLP_BREVIS: // mensural brevis head
      out = Font_interface::get_default_font (me)->find_by_name
	("noteheads.s-1mensural");
      break;
    case MLP_MAXIMA: // should be mensural maxima head without stem
      out = Font_interface::get_default_font (me)->find_by_name
	("noteheads.s-1neomensural");
      break;
    case MLP_FLEXA:
      out = brew_flexa (me, delta_pitch, false, width, thickness);
      break;
    default:
      programming_error (_f ("Mensural_ligature: "
			     "unexpected case fall-through"));
      return Stencil ();
    }

  Real blotdiameter
    = (me->get_layout ()->get_dimension (ly_symbol2scm ("blotdiameter")));

  if (primitive & MLP_STEM)
    {
      // assume MLP_UP
      Real y_bottom = 0.0, y_top = 3.0 * staff_space;

      if (primitive & MLP_DOWN)
	{
	  y_bottom = -y_top;
	  y_top = 0.0;
	}

      Interval x_extent (0, thickness);
      Interval y_extent (y_bottom, y_top);
      Box join_box (x_extent, y_extent);

      Stencil join = Lookup::round_filled_box (join_box, blotdiameter);
      out.add_stencil (join);
    }

  SCM join_right_scm = me->get_property ("join-right-amount");

  if (join_right_scm != SCM_EOL)
    {
      int join_right = scm_to_int (join_right_scm);
      if (join_right)
	{
	  Real y_top = join_right * 0.5 * staff_space;
	  Real y_bottom = 0.0;

	  if (y_top < 0.0)
	    {
	      y_bottom = y_top;
	      y_top = 0.0;
	    }

	  Interval x_extent (width - thickness, width);
	  Interval y_extent (y_bottom, y_top);
	  Box join_box (x_extent, y_extent);
	  Stencil join = Lookup::round_filled_box (join_box, blotdiameter);

	  out.add_stencil (join);
	}
      else
	programming_error (_f ("Mensural_ligature: (join_right == 0)"));
    }

#if 0 /* what happend with the ledger lines? */
  int pos = Staff_symbol_referencer::get_rounded_position (me);
  if (primitive & MLP_FLEXA)
    {
      pos += delta_pitch;
      add_ledger_lines (me, &out, pos, 0.5 * delta_pitch, ledger_take_space);
    }
#endif

  return out;
}

MAKE_SCHEME_CALLBACK (Mensural_ligature, brew_ligature_primitive, 1);
SCM
Mensural_ligature::brew_ligature_primitive (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  return internal_brew_primitive (me).smobbed_copy ();
}

MAKE_SCHEME_CALLBACK (Mensural_ligature, print, 1);
SCM
Mensural_ligature::print (SCM)
{
  return SCM_EOL;
}

ADD_INTERFACE (Mensural_ligature, "mensural-ligature-interface",
	       "A mensural ligature",
	       "delta-pitch flexa-width head-width join-right-amount " // "add-join "
	       "ligature-primitive-callback primitive thickness");
