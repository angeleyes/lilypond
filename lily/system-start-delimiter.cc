/*
  system-start-delimiter.cc -- implement System_start_delimiter

  source file of the GNU LilyPond music typesetter

  (c) 2000--2006 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "system-start-delimiter.hh"
#include "text-interface.hh" 
#include "all-font-metrics.hh"
#include "axis-group-interface.hh"
#include "font-interface.hh"
#include "item.hh"
#include "line-interface.hh"
#include "lookup.hh"
#include "output-def.hh"
#include "pointer-group-interface.hh"
#include "spanner.hh"
#include "staff-symbol-referencer.hh"

Stencil
System_start_delimiter::staff_bracket (Grob *me, Real height)
{
  SCM fam = scm_cons (ly_symbol2scm ("font-encoding"),
		      ly_symbol2scm ("fetaMusic"));

  SCM alist = scm_list_n (fam, SCM_UNDEFINED);
  Font_metric *fm = select_font (me->layout (), scm_list_n (alist, SCM_UNDEFINED));

  Drul_array<Stencil> tips (fm->find_by_name ("brackettips.down"),
			    fm->find_by_name ("brackettips.up"));

  Real thickness = robust_scm2double (me->get_property ("thickness"), 0.25);

  Real overlap = 0.1 * thickness;

  Box box (Interval (0, thickness),
	   Interval (-1, 1)
	   * (height / 2 + overlap));
  
  Stencil bracket = Lookup::filled_box (box);
  Direction d = DOWN;
  do
    bracket.add_at_edge (Y_AXIS, d, tips[d], -overlap, 0.0);
  while (flip (&d) != DOWN);
  bracket = Stencil (box, bracket.expr ());

  bracket.translate_axis (-0.8, X_AXIS);
  
  return bracket;
}

Stencil
System_start_delimiter::line_bracket (Grob *me, Real height)
{
  Real thick
    = me->layout ()->get_dimension (ly_symbol2scm ("line-thickness"))
    * robust_scm2double (me->get_property ("thickness"), 1);
  Real w = 0.8;
  
  Stencil tip1 = Line_interface::make_line (thick,
					   Offset (0, -height/2),
					   Offset (w, -height/2));
  Stencil tip2 = Line_interface::make_line (thick,
					    Offset (0, height/2),
					    Offset (w, height/2));
  Stencil vline = Line_interface::make_line (thick,
					     Offset (0, -height/2),
					     Offset (0, height/2));

  vline.add_stencil (tip1);
  vline.add_stencil (tip2);
  vline.translate_axis (-w, X_AXIS);
  return vline;
}

Stencil
System_start_delimiter::text (Grob *me_grob, Real h)
{
  (void) h;
  
  Spanner *me = dynamic_cast<Spanner*> (me_grob);
  SCM t = me->get_property ("text");
  if (me->get_break_index () == 0)
    t = me->get_property ("long-text");
	   
  
  SCM chain = Font_interface::text_font_alist_chain (me);

  SCM scm_stencil = Text_interface::is_markup (t)
    ? Text_interface::interpret_markup (me->layout ()->self_scm (), chain, t)
    : SCM_EOL;
  
  if (Stencil *p = unsmob_stencil (scm_stencil))
    return *p;
  return Stencil();
}

Stencil
System_start_delimiter::simple_bar (Grob *me, Real h)
{
  Real lt = me->layout ()->get_dimension (ly_symbol2scm ("line-thickness"));
  Real w = lt * robust_scm2double (me->get_property ("thickness"), 1);
  return Lookup::round_filled_box (Box (Interval (0, w), Interval (-h / 2, h / 2)),
				   lt);
}

MAKE_SCHEME_CALLBACK (System_start_delimiter, print, 1);
SCM
System_start_delimiter::print (SCM smob)
{
  Spanner *me = unsmob_spanner (smob);
  extract_grob_set (me, "elements", elts);
  Grob *common = common_refpoint_of_array (elts, me, Y_AXIS);

  Interval ext;
  int non_empty_count = 0;
  for (vsize i = elts.size (); i--;)
    {
      Spanner *sp = dynamic_cast<Spanner *> (elts[i]);

      if (sp
	  && sp->get_bound (LEFT) == me->get_bound (LEFT))
	{
	  Interval dims = sp->extent (common, Y_AXIS);
	  if (!dims.is_empty ())
	    {
	      non_empty_count ++;
	      ext.unite (dims);
	    }
	}
    }

  SCM glyph_sym = me->get_property ("style");
  Real len = ext.length ();
  if (ext.is_empty ()
      || (robust_scm2double (me->get_property ("collapse-height"), 0.0) >= ext.length ())
      || (glyph_sym == ly_symbol2scm ("bar-line")
	  && non_empty_count <= 1))
    {
      me->suicide ();
      return SCM_UNSPECIFIED;
    }

  Stencil m;
  if (glyph_sym == ly_symbol2scm ("bracket"))
    m = staff_bracket (me, len);
  else if (glyph_sym == ly_symbol2scm ("brace"))
    m = staff_brace (me, len);
  else if (glyph_sym == ly_symbol2scm ("bar-line"))
    m = simple_bar (me, len);
  else if (glyph_sym == ly_symbol2scm ("line-bracket"))
    m = line_bracket (me, len);
  else if (glyph_sym == ly_symbol2scm ("text"))
    m = text (me, len);

  m.translate_axis (ext.center (), Y_AXIS);
  return m.smobbed_copy ();
}

Stencil
System_start_delimiter::staff_brace (Grob *me, Real y)
{
  Font_metric *fm = 0;
  /* We go through the style sheet to lookup the font file
     name.  This is better than using find_font directly,
     esp. because that triggers mktextfm for non-existent
>     fonts. */
  SCM fam = scm_cons (ly_symbol2scm ("font-encoding"),
		      ly_symbol2scm ("fetaBraces"));

  SCM alist = scm_list_n (fam, SCM_UNDEFINED);
  fm = select_font (me->layout (), scm_list_n (alist, SCM_UNDEFINED));

  int
    lo = 0;
  int hi = max ((int) fm->count () - 1, 2);

  /* do a binary search for each Y, not very efficient, but passable?  */
  Box b;
  do
    {
      int cmp = (lo + hi) / 2;
      b = fm->get_indexed_char (cmp);
      if (b[Y_AXIS].is_empty () || b[Y_AXIS].length () > y)
	hi = cmp;
      else
	lo = cmp;
    }
  while (hi - lo > 1);

  Stencil stil (fm->find_by_name ("brace" + to_string (lo)));
  stil.translate_axis (-b[X_AXIS].length()/2, X_AXIS);

  stil.translate_axis (-0.2, X_AXIS);
  
  return stil;
}

ADD_INTERFACE (System_start_delimiter, "system-start-delimiter-interface",
	       "The brace, bracket or bar in front of the system. "
	       ,

	       /* properties */
	       "collapse-height "
	       "style "
	       "text "
	       "long-text " 
	       "thickness "
	       );
