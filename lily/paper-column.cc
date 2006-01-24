/*
  paper-column.cc -- implement Paper_column

  source file of the GNU LilyPond music typesetter

  (c) 1997--2006 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "paper-column.hh"

#include "moment.hh"
#include "paper-score.hh"
#include "warn.hh"
#include "axis-group-interface.hh"
#include "spaceable-grob.hh"
#include "text-interface.hh"
#include "lookup.hh"
#include "font-interface.hh"
#include "output-def.hh"
#include "pointer-group-interface.hh"
#include "grob-array.hh"

Grob *
Paper_column::clone (int count) const
{
  return new Paper_column (*this, count);
}

void
Paper_column::do_break_processing ()
{
  Spaceable_grob::remove_interface (this);
  Item::do_break_processing ();
}

int
Paper_column::get_rank (Grob *me)
{
  return dynamic_cast<Paper_column *> (me)->rank_;
}

System *
Paper_column::get_system () const
{
  return system_;
}

Paper_column *
Paper_column::get_column () const
{
  return (Paper_column *) (this);
}

Paper_column::Paper_column (SCM l, Object_key const *key)
  : Item (l, key)		// guh.?
{
  system_ = 0;
  rank_ = -1;
}

Paper_column::Paper_column (Paper_column const &src, int count)
  : Item (src, count)
{
  system_ = 0;
  rank_ = src.rank_;
}

Moment
Paper_column::when_mom (Grob *me)
{
  SCM m = me->get_property ("when");
  if (Moment *when = unsmob_moment (m))
    return *when;
  return Moment (0);
}

bool
Paper_column::is_musical (Grob *me)
{
  SCM m = me->get_property ("shortest-starter-duration");
  Moment s (0);
  if (unsmob_moment (m))
    s = *unsmob_moment (m);
  return s != Moment (0);
}

bool
Paper_column::is_used (Grob *me)
{
  extract_grob_set (me, "elements", elts);
  if (elts.size ())
    return true;

  extract_grob_set (me, "bounded-by-me", bbm);
  if (bbm.size ())
    return true;
  
  if (Item::is_breakable (me))
    return true;

  if (to_boolean (me->get_property ("used")))
    return true;
  return false;
}

/*
  Print a vertical line and  the rank number, to aid debugging.
*/

MAKE_SCHEME_CALLBACK (Paper_column, print, 1);
SCM
Paper_column::print (SCM p)
{
  Grob *me = unsmob_grob (p);

  String r = to_string (Paper_column::get_rank (me));

  Moment *mom = unsmob_moment (me->get_property ("when"));
  String when = mom ? mom->to_string () : "?/?";

  SCM properties = Font_interface::text_font_alist_chain (me);

  SCM scm_mol = Text_interface::interpret_markup (me->layout ()->self_scm (),
						  properties,
						  scm_makfrom0str (r.c_str ()));
  SCM when_mol = Text_interface::interpret_markup (me->layout ()->self_scm (),
						   properties,
						   scm_makfrom0str (when.c_str ()));
  Stencil t = *unsmob_stencil (scm_mol);
  t.add_at_edge (Y_AXIS, DOWN, *unsmob_stencil (when_mol), 0.1, 0.1);
  t.align_to (X_AXIS, CENTER);
  t.align_to (Y_AXIS, DOWN);

  Stencil l = Lookup::filled_box (Box (Interval (-0.01, 0.01),
				       Interval (-2, -1)));

  t.add_stencil (l);
  return t.smobbed_copy ();
}

/*
  This is all too hairy. We use bounded-by-me to make sure that some
  columns are kept "alive". Unfortunately, when spanners are suicided,
  this falls apart again, because suicided spanners are still in
  bounded-by-me

  THIS IS BROKEN KLUDGE. WE SHOULD INVENT SOMETHING BETTER.
*/
MAKE_SCHEME_CALLBACK (Paper_column, before_line_breaking, 1);
SCM
Paper_column::before_line_breaking (SCM grob)
{
  Grob *me = unsmob_grob (grob);

  SCM bbm = me->get_object ("bounded-by-me");
  Grob_array *ga = unsmob_grob_array (bbm);
  if (!ga)
    return SCM_UNSPECIFIED;

  Link_array<Grob> &array (ga->array_reference ());

  for (int i = array.size (); i--;)
    {
      Grob *g = array[i];

      if (!g || !g->is_live ())
	{			// UGH . potentially quadratic.
	  array.del (i);
	}
    }

  return SCM_UNSPECIFIED;
}


ADD_INTERFACE (Paper_column,

	       "paper-column-interface",
	       "@code{Paper_column} objects form the top-most X-parents for items."
	       "  The are two types of columns: musical columns, where are attached to, and "
	       "  non-musical columns, where bar-lines, clefs etc. are attached to. "
	       "  The spacing engine determines the X-positions of these objects."
	       
	       "\n\n"
	       "They are\n"
	       "  numbered, the first (leftmost) is column 0. Numbering happens before\n"
	       "  line-breaking, and columns are not renumbered after line breaking.\n"
	       "  Since many columns go unused, you should only use the rank field to\n"
	       "  get ordering information.  Two adjacent columns may have\n"
	       "  non-adjacent numbers.\n",
	       

	       /* properties */
	       "between-cols "
	       "bounded-by-me "
	       "line-break-system-details "
	       "page-penalty "
	       "shortest-playing-duration "
	       "shortest-starter-duration "
	       "used "
	       "when ");

