/*   
  time-signature.cc -- implement Time_signature
  
  source file of the GNU LilyPond music typesetter
  
  (c) 1996--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  
 */


#include "stencil.hh"
#include "text-item.hh"
#include "time-signature.hh"
#include "output-def.hh"
#include "font-interface.hh"
#include "warn.hh"
#include "staff-symbol-referencer.hh"


/*
  TODO:

  this file should go ; The formatting can completely be done with
  markups.
  
 */

MAKE_SCHEME_CALLBACK (Time_signature, print, 1);
SCM
Time_signature::print (SCM smob) 
{
  Grob * me = unsmob_grob (smob);
  SCM st = me->get_property ("style");
  SCM frac = me->get_property ("fraction");
  int n = 4;
  int d = 4;
  if (ly_c_pair_p (frac))
    {
      n = scm_to_int (ly_car (frac));
      d = scm_to_int (ly_cdr (frac));
    }

  Stencil m;
  if (st == ly_symbol2scm ("single-digit"))
    {
      m = numbered_time_signature (me, n, 0);
    }
  else if (scm_is_symbol (st))
    {
      m = special_time_signature (me, st, n, d);
    }
  else
    m = numbered_time_signature (me, n,d);

  if (Staff_symbol_referencer::line_count (me) % 2 == 0)
    m.translate_axis (Staff_symbol_referencer::staff_space (me)/2 , Y_AXIS);

  return m.smobbed_copy ();
}

Stencil
Time_signature::special_time_signature (Grob *me, SCM scm_style, int n, int d)
{
  String style = ly_scm2string (scm_symbol_to_string (scm_style));

  if (style == "numbered")
    return numbered_time_signature (me, n, d);

  if ((style == "default") || (style == ""))
    style = to_string ("C");

  if (style == "C")
    {
      if /* neither C2/2 nor C4/4 */
	(((n != 2) || (d != 2)) && 
	 ((n != 4) || (d != 4)))
	{
	  return numbered_time_signature (me, n, d);
	}
    }

  String char_name = style + to_string (n) + "/" + to_string (d);
  me->set_property ("font-encoding", ly_symbol2scm ("fetaMusic"));
  Stencil out = Font_interface::get_default_font (me)
    ->find_by_name ("timesig-" + char_name);
  if (!out.is_empty ())
    return out;

  /* If there is no such symbol, we default to the numbered style.
    (Here really with a warning!) */
  me->warning (_f ("time signature symbol `%s' not found; "
		   "reverting to numbered style", char_name));
  return numbered_time_signature (me, n, d);
}

Stencil
Time_signature::numbered_time_signature (Grob*me,int num, int den)
{
  SCM chain = me->get_property_alist_chain (Font_interface::text_font_alist_chain (me));
  chain = scm_cons (scm_list_1 (scm_cons (ly_symbol2scm ("font-encoding"),
					 ly_symbol2scm ("fetaNumber"))),
		    chain);
				
  SCM sn = Text_interface::interpret_markup (me->get_paper ()->self_scm (), chain,
					scm_makfrom0str (to_string (num).to_str0 ()));
  SCM sd = Text_interface::interpret_markup (me->get_paper ()->self_scm (), chain,
					scm_makfrom0str (to_string (den).to_str0 ()));

  Stencil n = *unsmob_stencil (sn);
  Stencil d = *unsmob_stencil (sd);
			      
  n.align_to (X_AXIS, CENTER);
  d.align_to (X_AXIS, CENTER);
  Stencil m;
  if (den)
    {
      m.add_at_edge (Y_AXIS, UP, n, 0.0, 0);
      m.add_at_edge (Y_AXIS, DOWN, d, 0.0,0);
    }
  else
    {
      m = n;
      m.align_to (Y_AXIS, CENTER);
    }

  m.align_to (X_AXIS, LEFT);
  
  return m;
}

ADD_INTERFACE (Time_signature, "time-signature-interface",
  "A time signature, in different styles.\n"
"  The following values for 'style are are recognized:\n"
"\n"
"    @table @samp\n"
"      @item @code{C}\n"
"        4/4 and 2/2 are typeset as C and struck C, respectively.  All\n"
"        other time signatures are written with two digits.\n"
"\n"
"      @item @code{neomensural}\n"
"        2/2, 3/2, 2/4, 3/4, 4/4, 6/4, 9/4, 4/8, 6/8 and 9/8 are\n"
"        typeset with neo-mensural style mensuration marks.  All other time\n"
"        signatures are written with two digits.\n"
"\n"
"      @item @code{mensural}\n"
"        2/2, 3/2, 2/4, 3/4, 4/4, 6/4, 9/4, 4/8, 6/8 and 9/8 are\n"
"        typeset with mensural style mensuration marks.  All other time\n"
"        signatures are written with two digits.\n"
"\n"
"      @item @code{single-digit}\n"
"        All time signatures are typeset with a single\n"
"        digit, e.g. 3/2 is written as 3.\n"
"    @end table\n"
"\n"
"See also the test-file @file{input/test/time.ly}.\n",
  "fraction style");
