/*
  accidental.cc -- implement Accidental_interface

  source file of the GNU LilyPond music typesetter

  (c) 2001--2006 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "accidental-interface.hh"
#include "font-interface.hh"
#include "international.hh"
#include "item.hh"
#include "output-def.hh"
#include "pitch.hh"
#include "stencil.hh"

Stencil
parenthesize (Grob *me, Stencil m)
{
  Font_metric * font
    = Font_interface::get_default_font (me); 
  Stencil open
    = font->find_by_name ("accidentals.leftparen");
  Stencil close
    = font->find_by_name ("accidentals.rightparen");

  m.add_at_edge (X_AXIS, LEFT, Stencil (open), 0, 0);
  m.add_at_edge (X_AXIS, RIGHT, Stencil (close), 0, 0);

  return m;
}

/*
  Hmm. Need separate callback, or perhaps #'live bool property.
 */
MAKE_SCHEME_CALLBACK (Accidental_interface, after_line_breaking, 1);
SCM
Accidental_interface::after_line_breaking (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  Grob *tie = unsmob_grob (me->get_object ("tie"));

  if (tie && !tie->original ()
      && !to_boolean (me->get_property ("forced")))
    {
      me->suicide ();
    }
 
  return SCM_UNSPECIFIED;
}

vector<Box>
Accidental_interface::accurate_boxes (Grob *me, Grob **common)
{
  Box b;
  b[X_AXIS] = me->extent (me, X_AXIS);
  b[Y_AXIS] = me->extent (me, Y_AXIS);

  vector<Box> boxes;

  bool parens = to_boolean (me->get_property ("parenthesized"));

  SCM scm_style = me->get_property ("style");
  if (!scm_is_symbol (scm_style)
      && !to_boolean (me->get_property ("restore-first"))
      && !parens)
    {
      Rational alteration = ly_scm2rational (me->get_property ("alteration"));
      if (alteration == FLAT_ALTERATION)
	{
	  Box stem = b;
	  Box bulb = b;

	  /*
	    we could make the stem thinner, but that places the flats
	    really close.
	  */
	  stem[X_AXIS][RIGHT] *= .5;

	  /*
	    To prevent vertical alignment for 6ths
	  */
	  stem[Y_AXIS] *= 1.1;
	  bulb[Y_AXIS][UP] *= .35;

	  boxes.push_back (bulb);
	  boxes.push_back (stem);
	}
      else if (alteration == NATURAL_ALTERATION)
	{
	  Box lstem = b;
	  Box rstem = b;
	  Box belly = b;

	  lstem[Y_AXIS] *= 1.1;
	  rstem[Y_AXIS] *= 1.1;

	  belly[Y_AXIS] *= 0.75;
	  lstem[X_AXIS][RIGHT] *= .33;
	  rstem[X_AXIS][LEFT] = rstem[X_AXIS].linear_combination (1.0 / 3.0);
	  lstem[Y_AXIS][DOWN] = belly[Y_AXIS][DOWN];
	  rstem[Y_AXIS][UP] = belly[Y_AXIS][UP];
	  boxes.push_back (belly);
	  boxes.push_back (lstem);
	  boxes.push_back (rstem);
	}
      /*
	TODO: add support for, double flat.
      */
    }

  if (!boxes.size ())
    boxes.push_back (b);

  Offset o (me->relative_coordinate (common[X_AXIS], X_AXIS),
	    me->relative_coordinate (common[Y_AXIS], Y_AXIS));

  for (vsize i = boxes.size (); i--;)
    boxes[i].translate (o);

  return boxes;
}

MAKE_SCHEME_CALLBACK (Accidental_interface, print, 1);
SCM
Accidental_interface::print (SCM smob)
{
  Grob *me = unsmob_grob (smob);

  Font_metric *fm = Font_interface::get_default_font (me);

  SCM alist = me->get_property ("glyph-name-alist");
  SCM alt = me->get_property ("alteration");
  SCM glyph_name = ly_assoc_get (alt, alist, SCM_BOOL_F);
  
  if (!scm_is_string (glyph_name))
    {
      me->warning (_f ("Could not find glyph-name for alteration %s",
		       ly_scm2rational (alt).to_string ().c_str ()));
      return SCM_EOL;
    }
  
  Stencil mol (fm->find_by_name (scm_i_string_chars (glyph_name)));
  if (to_boolean (me->get_property ("restore-first")))
    {
      /*
	this isn't correct for ancient accidentals, but they don't
	use double flats/sharps anyway.
	*/
      Stencil acc (fm->find_by_name ("accidentals.natural"));

      if (acc.is_empty ())
	me->warning (_ ("natural alteration glyph not found"));
      else
	mol.add_at_edge (X_AXIS, LEFT, acc, 0.1, 0);
    }
  
  if (to_boolean (me->get_property ("parenthesized")))
    mol = parenthesize (me, mol);

  return mol.smobbed_copy ();
}

  
ADD_INTERFACE (Accidental_interface,
	       "a single accidental",
	       
	       /* props */
	       "alteration "
	       "avoid-slur "
	       "forced "
	       "parenthesized "
	       "restore-first "
	       "glyph-name-alist "
	       "tie "
	       );