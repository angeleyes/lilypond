/*
  multi-measure-rest.cc -- implement Multi_measure_rest

  source file of the GNU LilyPond music typesetter

  (c) 1998--2005 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include "multi-measure-rest.hh"

#include "warn.hh"
#include "output-def.hh"
#include "paper-column.hh" // urg
#include "font-interface.hh"
#include "rest.hh"
#include "misc.hh"
#include "spanner.hh"
#include "staff-symbol-referencer.hh"
#include "text-item.hh"
#include "percent-repeat-item.hh"
#include "lookup.hh"
#include "separation-item.hh"

MAKE_SCHEME_CALLBACK (Multi_measure_rest, percent, 1);
SCM
Multi_measure_rest::percent (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  Spanner *sp = dynamic_cast<Spanner *> (me);

  Stencil r = Percent_repeat_item_interface::x_percent (me, 1, 0.75, 1.6);

  // ugh copy & paste.

  Interval sp_iv;
  Direction d = LEFT;
  do
    {
      Item *col = sp->get_bound (d)->get_column ();

      Interval coldim = col->extent (0, X_AXIS);

      sp_iv[d] = coldim[-d];
    }
  while ((flip (&d)) != LEFT);
  Real x_off = 0.0;

  Real rx = sp->get_bound (LEFT)->relative_coordinate (0, X_AXIS);
  /*
    we gotta stay clear of sp_iv, so move a bit to the right if
    needed.
  */
  x_off += max (sp_iv[LEFT] - rx, 0.0);

  /*
    center between stuff.
  */
  x_off += sp_iv.length () / 2;

  r.translate_axis (x_off, X_AXIS);

  return r.smobbed_copy ();
}

MAKE_SCHEME_CALLBACK (Multi_measure_rest, print, 1);
SCM
Multi_measure_rest::print (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  Spanner *sp = dynamic_cast<Spanner *> (me);

  Interval sp_iv;
  Direction d = LEFT;

  Grob *common = sp->get_bound (LEFT)->common_refpoint (sp->get_bound (RIGHT), X_AXIS);
  do
    {
      Item *b = sp->get_bound (d);

      Interval coldim = (Separation_item::has_interface (b))
	? Separation_item::relative_width (b, common)
	: b->extent (common, X_AXIS);

      sp_iv[d] = coldim.is_empty () ? b->relative_coordinate (common, X_AXIS) : coldim[-d];
    }
  while ((flip (&d)) != LEFT);

  Real space = sp_iv.length ();

  Real rx = sp->get_bound (LEFT)->relative_coordinate (0, X_AXIS);
  /*
    we gotta stay clear of sp_iv, so move a bit to the right if
    needed.
  */
  Real x_off = max (sp_iv[LEFT] - rx, 0.0);

  Stencil mol;
  mol.add_stencil (symbol_stencil (me, space));

  int measures = 0;
  SCM m (me->get_property ("measure-count"));
  if (scm_is_number (m))
    {
      measures = scm_to_int (m);
    }

  mol.translate_axis (x_off, X_AXIS);
  return mol.smobbed_copy ();
}

Stencil
Multi_measure_rest::symbol_stencil (Grob *me, Real space)
{
  int measures = 0;
  SCM m (me->get_property ("measure-count"));
  if (scm_is_number (m))
    {
      measures = scm_to_int (m);
    }
  if (measures <= 0)
    return Stencil ();

  SCM limit = me->get_property ("expand-limit");
  if (measures > scm_to_int (limit))
    {
      Real padding = 0.15;
      Stencil s = big_rest (me, (1.0 - 2 * padding) * space);
      s.translate_axis (padding * space, X_AXIS);
      return s;
    }

  SCM alist_chain = Font_interface::music_font_alist_chain (me);

  Real staff_space = Staff_symbol_referencer::staff_space (me);
  Font_metric *musfont
    = select_font (me->get_layout (), alist_chain);

  SCM sml = me->get_property ("use-breve-rest");
  if (measures == 1)
    {
      if (to_boolean (sml))
	{
	  Stencil s = musfont->find_by_name (Rest::glyph_name (me, -1, "", false));

	  s.translate_axis ((space - s.extent (X_AXIS).length ()) / 2, X_AXIS);

	  return s;
	}
      else
	{
	  Stencil s = musfont->find_by_name (Rest::glyph_name (me, 0, "", true));

	  /*
	    ugh.
	  */
	  if (Staff_symbol_referencer::get_position (me) == 0.0)
	    s.translate_axis (staff_space, Y_AXIS);

	  s.translate_axis ((space - s.extent (X_AXIS).length ()) / 2, X_AXIS);

	  return s;
	}
    }
  else
    {
      return church_rest (me, musfont, measures, space);
    }
}

/*
  WIDTH can also be 0 to determine the minimum size of the object.
*/
Stencil
Multi_measure_rest::big_rest (Grob *me, Real width)
{
  Real thick_thick = robust_scm2double (me->get_property ("thick-thickness"), 1.0);
  Real hair_thick = robust_scm2double (me->get_property ("hair-thickness"), .1);

  Real ss = Staff_symbol_referencer::staff_space (me);
  Real slt = me->get_layout ()->get_dimension (ly_symbol2scm ("linethickness"));
  Real y = slt * thick_thick / 2 * ss;
  Real ythick = hair_thick * slt * ss;
  Box b (Interval (0.0, max (0.0, (width - 2 * ythick))), Interval (-y, y));

  Real blot = width ? (.8 * min (y, ythick)) : 0.0;

  Stencil m = Lookup::round_filled_box (b, blot);
  Stencil yb = Lookup::round_filled_box (Box (Interval (-0.5, 0.5) * ythick, Interval (-ss, ss)), blot);

  m.add_at_edge (X_AXIS, RIGHT, yb, 0, 0);
  m.add_at_edge (X_AXIS, LEFT, yb, 0, 0);

  m.align_to (X_AXIS, LEFT);

  return m;
}

/*
  Kirchenpause (?)
*/
Stencil
Multi_measure_rest::church_rest (Grob *me, Font_metric *musfont, int measures,
				 Real space)
{
  SCM mols = SCM_EOL;

  /* See Wanske pp. 125  */
  int l = measures;
  int count = 0;
  Real symbols_width = 0.0;

  SCM sml = me->get_property ("use-breve-rest");

  while (l)
    {
      if (sml == SCM_BOOL_T)
	{
	  int k;
	  if (l >= 2)
	    {
	      l -= 2;
	      k = -2;
	    }
	  else
	    {
	      l -= 1;
	      k = -1;
	    }

	  Stencil r (musfont->find_by_name ("rests." + to_string (k)));
	  symbols_width += r.extent (X_AXIS).length ();
	  mols = scm_cons (r.smobbed_copy (), mols);
	}
      else
	{
	  int k;
	  if (l >= 4)
	    {
	      l -= 4;
	      k = -2;
	    }
	  else if (l >= 2)
	    {
	      l -= 2;
	      k = -1;
	    }
	  else
	    {
	      k = 0;
	      l--;
	    }

	  Stencil r (musfont->find_by_name ("rests." + to_string (k)));
	  if (k == 0)
	    {
	      Real staff_space = Staff_symbol_referencer::staff_space (me);
	      r.translate_axis (staff_space, Y_AXIS);
	    }
	  symbols_width += r.extent (X_AXIS).length ();
	  mols = scm_cons (r.smobbed_copy (), mols);
	}
      count++;
    }

  /* Make outer padding this much bigger.  */
  Real outer_padding_factor = 1.5;
  Real inner_padding = (space - symbols_width)
    / (2 * outer_padding_factor + (count - 1));
  if (inner_padding < 0)
    inner_padding = 1.0;

  Stencil mol;
  for (SCM s = mols; scm_is_pair (s); s = scm_cdr (s))
    mol.add_at_edge (X_AXIS, LEFT, *unsmob_stencil (scm_car (s)),
		     inner_padding, 0);
  mol.align_to (X_AXIS, LEFT);
  mol.translate_axis (outer_padding_factor * inner_padding, X_AXIS);

  return mol;
}

void
Multi_measure_rest::add_column (Grob *me, Item *c)
{
  add_bound_item (dynamic_cast<Spanner *> (me), c);
}

MAKE_SCHEME_CALLBACK (Multi_measure_rest, set_spacing_rods, 1);
SCM
Multi_measure_rest::set_spacing_rods (SCM smob)
{
  Grob *me = unsmob_grob (smob);

  Spanner *sp = dynamic_cast<Spanner *> (me);
  if (! (sp->get_bound (LEFT) && sp->get_bound (RIGHT)))
    {
      programming_error ("Multi_measure_rest::get_rods (): I am not spanned!");
      return SCM_UNSPECIFIED;
    }

  Item *li = sp->get_bound (LEFT)->get_column ();
  Item *ri = sp->get_bound (RIGHT)->get_column ();
  Item *lb = li->find_prebroken_piece (RIGHT);
  Item *rb = ri->find_prebroken_piece (LEFT);

  Item *combinations[4][2] = {{li, ri},
			      {lb, ri},
			      {li, rb},
			      {lb, rb}};

  Real sym_width = symbol_stencil (me, 0.0).extent (X_AXIS).length ();

  for (int i = 0; i < 4; i++)
    {
      Item *li = combinations[i][0];
      Item *ri = combinations[i][1];

      if (!li || !ri)
	continue;

      Rod rod;
      rod.item_drul_[LEFT] = li;
      rod.item_drul_[RIGHT] = ri;

      rod.distance_ = li->extent (li, X_AXIS)[BIGGER]
	- ri->extent (ri, X_AXIS)[SMALLER]
	/* 2.0 = magic! */
	+ sym_width + 2.0;

      Real minlen = robust_scm2double (me->get_property ("minimum-length"), 0);
      rod.distance_ = max (rod.distance_, minlen);
      rod.add_to_cols ();
    }
  return SCM_UNSPECIFIED;
}

ADD_INTERFACE (Multi_measure_rest, "multi-measure-rest-interface",
	       "A rest that spans a whole number of measures.",
	       "expand-limit measure-count hair-thickness thick-thickness use-breve-rest minimum-length");

