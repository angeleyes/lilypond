/*
  bar.cc -- implement Bar

  source file of the GNU LilyPond music typesetter

  (c) 1997--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include <math.h>

#include "lookup.hh"
#include "paper-column.hh"
#include "grob.hh"
#include "bar-line.hh"
#include "string.hh"
#include "molecule.hh"
#include "paper-def.hh"
#include "font-interface.hh"

#include "all-font-metrics.hh"
#include "item.hh"
#include "staff-symbol-referencer.hh"

MAKE_SCHEME_CALLBACK (Bar_line,print,1);

SCM 
Bar_line::print (SCM smob) 
{
  Grob * me = unsmob_grob (smob);

  SCM s = me->get_grob_property ("glyph");
  SCM barsiz_proc = me->get_grob_property ("bar-size-procedure");
  if (gh_string_p (s) && gh_procedure_p (barsiz_proc))
    {
      String str  =ly_scm2string (s);
      SCM siz = gh_call1 (barsiz_proc, me->self_scm ());
      Real sz = robust_scm2double (siz, 0);
      if (sz < 0)
	return SCM_EOL;
      
      return compound_barline (me, str, sz).smobbed_copy ();
    }
  return SCM_EOL;
}


Molecule
Bar_line::compound_barline (Grob*me, String str, Real h)
{
  Real kern = robust_scm2double (me->get_grob_property ("kern"), 1);
  Real thinkern = robust_scm2double (me->get_grob_property ("thin-kern"), 1);
  Real hair = robust_scm2double (me->get_grob_property ("hair-thickness"), 1);
  Real fatline = robust_scm2double (me->get_grob_property ("thick-thickness"), 1);

  Real staffline = me->get_paper ()->get_realvar (ly_symbol2scm ("linethickness"));
  Real staff_space = Staff_symbol_referencer::staff_space (me);

  kern *= staffline;
  thinkern *= staffline;
  hair *= staffline;
  fatline *= staffline;
  
  Molecule thin = simple_barline (me, hair, h);
  Molecule thick = simple_barline (me, fatline, h);
  Molecule dot = Font_interface::get_default_font (me)->find_by_name ("dots-dot");
  Real dist = ( Staff_symbol_referencer::line_count (me) & 1 ? 1 :
		(staff_space<2 ? 2 : .5) ) * staff_space;
  Molecule colon (dot);
  colon.translate_axis(dist,Y_AXIS);
  colon.add_molecule(dot);
  colon.translate_axis(-dist/2,Y_AXIS);

  Molecule m;
  
  if (str == "")
    {
      return Lookup::blank (Box (Interval (0, 0), Interval (-h/2, h/2)));
    }
  else if (str == "|")
    {
      return thin;
    }
  else if (str == "|." || (h == 0 && str == ":|"))
    {
      m.add_at_edge (X_AXIS, LEFT, thick, 0, 0);      
      m.add_at_edge (X_AXIS, LEFT, thin, kern,0 );
    }
  else if (str == ".|" || (h == 0 && str == "|:"))
    {
      m.add_at_edge (X_AXIS, RIGHT, thick, 0, 0);
      m.add_at_edge (X_AXIS, RIGHT, thin, kern, 0);
    }
  else if (str == ":|")
    {
      m.add_at_edge (X_AXIS, LEFT, thick, 0, 0);
      m.add_at_edge (X_AXIS, LEFT, thin, kern, 0);
      m.add_at_edge (X_AXIS, LEFT, colon, kern, 0);      
    }
  else if (str == "|:")
    {
      m.add_at_edge (X_AXIS, RIGHT, thick, 0, 0);
      m.add_at_edge (X_AXIS, RIGHT, thin, kern, 0);
      m.add_at_edge (X_AXIS, RIGHT, colon, kern, 0);      
    }
  else if (str == ":|:")
    {
      m.add_at_edge (X_AXIS, LEFT, thick, thinkern, 0);
      m.add_at_edge (X_AXIS, LEFT, colon, kern, 0);
      m.add_at_edge (X_AXIS, RIGHT, thick, kern, 0);
      m.add_at_edge (X_AXIS, RIGHT, colon, kern, 0);      
    }
  else if (str == ".|.")
    {
      m.add_at_edge (X_AXIS, LEFT, thick, thinkern, 0);
      m.add_at_edge (X_AXIS, RIGHT, thick, kern, 0);
    }
  else if (str == "||")
    {
      /*
	should align to other side? this never appears
	on the system-start?
      */
      m.add_at_edge (X_AXIS, RIGHT, thin, 0, 0);
      m.add_at_edge (X_AXIS, RIGHT, thin, thinkern, 0);
    }
  else if (str == ":")
    {
      int c = (Staff_symbol_referencer::line_count (me));
      
      for (int i = 0 ; i < c - 1; i++)
	{
	  Real y = (- (c-1.0) / 2 + 0.5  +   i * staff_space);
	  Molecule d (dot);

	  d. translate_axis (y,Y_AXIS);
	  m.add_molecule (d);
	}
    }
  return m;
}

Molecule
Bar_line::simple_barline (Grob *me,Real w, Real h) 
{
  Real blot = me->get_paper ()->get_realvar (ly_symbol2scm ("blotdiameter"));
  return Lookup::round_filled_box (Box (Interval (0,w), Interval (-h/2, h/2)), blot);
}

MAKE_SCHEME_CALLBACK (Bar_line,before_line_breaking ,1);

SCM
Bar_line::before_line_breaking (SCM smob)
{
  Grob*me=unsmob_grob (smob);
  Item * item = dynamic_cast<Item*> (me);
  
  SCM g = me->get_grob_property ("glyph");
  SCM orig = g;
  Direction bsd = item->break_status_dir ();
  if (gh_string_p (g) && bsd)
    {
      SCM proc = me->get_grob_property ("break-glyph-function");
      g = gh_call2 (proc, g, scm_int2num (bsd));
    }
  
  if (!gh_string_p (g))
    {
      me->set_grob_property ("print-function", SCM_EOL);
      me->set_extent (SCM_EOL, X_AXIS);
      // leave y_extent for spanbar? 
    }

  if (! gh_equal_p (g, orig))
    me->set_grob_property ("glyph", g);

  return SCM_UNSPECIFIED;
}
  



MAKE_SCHEME_CALLBACK (Bar_line,get_staff_bar_size,1);
SCM
Bar_line::get_staff_bar_size (SCM smob) 
{
  Grob*me = unsmob_grob (smob);
  Real ss = Staff_symbol_referencer::staff_space (me);
  SCM size = me->get_grob_property ("bar-size");
  if (gh_number_p (size))
    return gh_double2scm (gh_scm2double (size)*ss);
  else if (Staff_symbol_referencer::get_staff_symbol (me))
    {
      /*
	If there is no staff-symbol, we get -1 from the next
	calculation. That's a nonsense value, which would collapse the
	barline so we return 0.0 in the next alternative.
      */
      return gh_double2scm ((Staff_symbol_referencer::line_count (me) -1) * ss);
    }
  else
    return scm_int2num (0);
}



ADD_INTERFACE (Bar_line, "bar-line-interface",
  "Bar line.\n"
"\n"
"Print a special bar symbol. It replaces the \n"
"regular bar symbol with a special\n"
"symbol.  The argument @var{bartype} is a string which specifies the\n"
"kind of bar to print.  Options are @code{:|},\n"
"@code{|:}, @code{:|:},\n"
"@code{||}, @code{|.},\n"
"@code{.|}, and @code{.|.}. \n"
"\n"
"These produce, respectively, a right repeat, a left repeat, a double\n"
"repeat, a double bar, a start bar, an end bar, and a thick double bar.\n"
"If @var{bartype} is set to @code{empty} then nothing is printed,\n"
"but a line break is allowed at that spot.\n",
  "bar-size-procedure kern thin-kern hair-thickness thick-thickness glyph bar-size break-glyph-function");
