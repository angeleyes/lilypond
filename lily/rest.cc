/*
  rest.cc -- implement Rest

  source file of the GNU LilyPond music typesetter

  (c) 1997--2006 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "rest.hh"

#include "stencil.hh"
#include "output-def.hh"
#include "font-interface.hh"
#include "dots.hh"
#include "paper-score.hh"
#include "staff-symbol-referencer.hh"
#include "directional-element-interface.hh"

// -> offset callback
MAKE_SCHEME_CALLBACK (Rest, y_offset_callback, 1);
SCM
Rest::y_offset_callback (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  int duration_log = scm_to_int (me->get_property ("duration-log"));
  int line_count = Staff_symbol_referencer::line_count (me);
  Real ss = Staff_symbol_referencer::staff_space (me);

  Real amount = robust_scm2double (me->get_property ("staff-position"), 0)
    * 0.5 * ss;
  if (line_count % 2)
    {
      if (duration_log == 0 && line_count > 1)
	amount += ss;
    }
  else
    amount += ss / 2;

  Grob *dot = unsmob_grob (me->get_object ("dot"));
  if (dot && duration_log > 4) // UGH.
    {
      dot->set_property ("staff-position",
		       scm_from_int ((duration_log == 7) ? 4 : 3));
    }
  if (dot && duration_log >= -1 && duration_log <= 1) // UGH again.
    {
      dot->set_property ("staff-position",
		       scm_from_int ((duration_log == 0) ? -1 : 1));
    }

  amount += 2 * ss * get_grob_direction (me);; 
  
  return scm_from_double (amount);
}

/*
  make this function easily usable in C++
*/
String
Rest::glyph_name (Grob *me, int balltype, String style, bool try_ledgers)
{
  bool is_ledgered = false;
  if (try_ledgers && (balltype == 0 || balltype == 1))
    {
      Real rad = Staff_symbol_referencer::staff_radius (me) * 2.0;
      Real pos = Staff_symbol_referencer::get_position (me);

      /*
	Figure out when the rest is far enough outside the staff. This
	could bemore generic, but hey, we understand this even after
	dinner.
      */
      is_ledgered |= (balltype == 0) && (pos >= +rad + 2 || pos < -rad);
      is_ledgered |= (balltype == 1) && (pos <= -rad - 2 || pos > +rad);
    }

  String actual_style (style.to_str0 ());

  if ((style == "mensural") || (style == "neomensural"))
    {

      /*
	FIXME: Currently, ancient font does not provide ledgered rests;
	hence the "o" suffix in the glyph name is bogus.  But do we need
	ledgered rests at all now that we can draw ledger lines with
	variable width, length and blotdiameter? -- jr
      */
      is_ledgered = 0;

      /*
	There are no 32th/64th/128th mensural/neomensural rests.  In
	these cases, revert back to default style.
      */
      if (balltype > 4)
	actual_style = "";
    }

  if ((style == "classical") && (balltype != 2))
    {
      /*
	classical style: revert back to default style for any rest other
	than quarter rest
      */
      actual_style = "";
    }

  if (style == "default")
    {
      /*
	Some parts of lily still prefer style "default" over "".
	Correct this here. -- jr
      */
      actual_style = "";
    }

  return ("rests." + to_string (balltype) + (is_ledgered ? "o" : "")
	  + actual_style);
}

MAKE_SCHEME_CALLBACK (Rest, print, 1);
SCM
Rest::brew_internal_stencil (Grob *me, bool ledgered)
{
  SCM balltype_scm = me->get_property ("duration-log");
  if (!scm_is_number (balltype_scm))
    return Stencil ().smobbed_copy ();

  int balltype = scm_to_int (balltype_scm);

  String style;
  SCM style_scm = me->get_property ("style");
  if (scm_is_symbol (style_scm))
    style = ly_scm2string (scm_symbol_to_string (style_scm));

  Font_metric *fm = Font_interface::get_default_font (me);
  String font_char = glyph_name (me, balltype, style, ledgered);
  Stencil out = fm->find_by_name (font_char);
  if (out.is_empty ())
    me->warning (_f ("rest `%s' not found", font_char.to_str0 ()));

  return out.smobbed_copy ();
}

SCM
Rest::print (SCM smob)
{
  return brew_internal_stencil (unsmob_grob (smob), true);
}

MAKE_SCHEME_CALLBACK (Rest, width, 1);
/*
  We need the callback. The real stencil has ledgers depending on
  Y-position. The Y-position is known only after line breaking.  */
SCM
Rest::width (SCM smob)
{
  return generic_extent_callback (unsmob_grob (smob), X_AXIS);
}

MAKE_SCHEME_CALLBACK (Rest, height, 1);
SCM
Rest::height (SCM smob)
{
  return generic_extent_callback (unsmob_grob (smob), Y_AXIS);
}

/*
  We need the callback. The real stencil has ledgers depending on
  Y-position. The Y-position is known only after line breaking.  */
SCM
Rest::generic_extent_callback (Grob *me, Axis a)
{
  /*
    Don't want ledgers: ledgers depend on Y position, which depends on
    rest collision, which depends on stem size which depends on beam
    slop of opposite note column.

    consequence: we get too small extents and potential collisions
    with ledgered rests.
  */
  SCM m = brew_internal_stencil (me, a != X_AXIS);
  return ly_interval2scm (unsmob_stencil (m)->extent (a));
}

ADD_INTERFACE (Rest, "rest-interface",
	       "A rest symbol.",

	       /* properties */
		  
	       "style "
	       "direction "
	       "minimum-distance");

