/*
  hara-kiri-vertical-group-spanner.cc -- implement Hara_kiri_group_spanner

  source file of the GNU LilyPond music typesetter

  (c) 1998--2007 Jan Nieuwenhuizen <janneke@gnu.org>
  Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "hara-kiri-group-spanner.hh"

#include "paper-column.hh"
#include "pointer-group-interface.hh"
#include "axis-group-interface.hh"
#include "spanner.hh"
#include "warn.hh"
#include "item.hh"

MAKE_SCHEME_CALLBACK (Hara_kiri_group_spanner, y_extent, 1);
SCM
Hara_kiri_group_spanner::y_extent (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  consider_suicide (me);
  return Axis_group_interface::generic_group_extent (me, Y_AXIS);
}

MAKE_SCHEME_CALLBACK (Hara_kiri_group_spanner, pure_height, 3);
SCM
Hara_kiri_group_spanner::pure_height (SCM smob, SCM start_scm, SCM end_scm)
{
  Grob *me = unsmob_grob (smob);
  int start = robust_scm2int (start_scm, 0);
  int end = robust_scm2int (end_scm, INT_MAX);

  if (request_suicide (me, start, end))
    return ly_interval2scm (Interval ());
  return Axis_group_interface::pure_group_height (me, start, end);
}

/* there is probably a way that doesn't involve re-implementing a binary
   search (I would love some proper closures right now) */
bool find_in_range (SCM vector, int low, int hi, int min, int max)
{
  if (low >= hi)
    return false;

  int mid = low + (hi - low) / 2;
  int val = scm_to_int (scm_c_vector_ref (vector, mid));
  if (val >= min && val <= max)
    return true;
  else if (val < min)
    return find_in_range (vector, mid+1, hi, min, max);
  return find_in_range (vector, low, mid, min, max);
}

bool
Hara_kiri_group_spanner::request_suicide (Grob *me, int start, int end)
{
  if (!to_boolean (me->get_property ("remove-empty")))
    return false;

  bool remove_first = to_boolean (me->get_property ("remove-first"));
  if (!remove_first && start <= 0)
    return false;

  SCM important = me->get_property ("important-column-ranks");
  if (scm_is_vector (important))
    {
      int len = scm_c_vector_length (important);
      if (find_in_range (important, 0, len, start, end))
	return false;
    }
  else /* build the important-columns-cache */
    {
      extract_grob_set (me, "items-worth-living", worth);
      vector<int> ranks;

      for (vsize i = 0; i < worth.size (); i++)
	{
	  Interval_t<int> iv = worth[i]->spanned_rank_iv ();
	  for (int j = iv[LEFT]; j <= iv[RIGHT]; j++)
	    ranks.push_back (j);
	}
      vector_sort (ranks, less<int> ());
      uniq (ranks);

      SCM scm_vec = scm_c_make_vector (ranks.size (), SCM_EOL);
      for (vsize i = 0; i < ranks.size (); i++)
	scm_vector_set_x (scm_vec, scm_from_int (i), scm_from_int (ranks[i]));
      me->set_property ("important-column-ranks", scm_vec);

      return request_suicide (me, start, end);
    }

  return true;
}

void
Hara_kiri_group_spanner::consider_suicide (Grob *me)
{
  Spanner *sp = dynamic_cast<Spanner*> (me);
  int left = sp->get_bound (LEFT)->get_column ()->get_rank ();
  int right = sp->get_bound (RIGHT)->get_column ()->get_rank ();
  if (!request_suicide (me, left, right))
    return;

  vector<Grob*> childs;
  Axis_group_interface::get_children (me, &childs);
  for (vsize i = 0; i < childs.size (); i++)
    childs[i]->suicide ();

  /*
    very appropriate name here :-)
  */
  me->suicide ();
}

/*
  We can't rely on offsets and dimensions of elements in a hara-kiri
  group. Use a callback to make sure that hara-kiri has been done
  before asking for offsets.  */
MAKE_SCHEME_CALLBACK (Hara_kiri_group_spanner, after_line_breaking, 1);
SCM
Hara_kiri_group_spanner::after_line_breaking (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  consider_suicide (me);
  return SCM_UNSPECIFIED;
}

MAKE_SCHEME_CALLBACK (Hara_kiri_group_spanner, force_hara_kiri_callback, 1);
SCM
Hara_kiri_group_spanner::force_hara_kiri_callback (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  consider_suicide (me);
  return scm_from_double (0.0);
}

MAKE_SCHEME_CALLBACK (Hara_kiri_group_spanner, force_hara_kiri_in_y_parent_callback, 1);
SCM
Hara_kiri_group_spanner::force_hara_kiri_in_y_parent_callback (SCM smob)
{
  Grob *daughter = unsmob_grob (smob);
  force_hara_kiri_callback (daughter->get_parent (Y_AXIS)->self_scm ());
  return scm_from_double (0.0);
}

void
Hara_kiri_group_spanner::add_interesting_item (Grob *me, Grob *n)
{
  Pointer_group_interface::add_unordered_grob (me, ly_symbol2scm ("items-worth-living"), n);
}

ADD_INTERFACE (Hara_kiri_group_spanner, "hara-kiri-group-interface",
	       "A group spanner that  keeps track of interesting items.  If it "
	       "doesn't contain any after linebreaking, then it "
	       "will remove itself and all its children.",


	       /* properties */
	       "items-worth-living "
	       "important-column-ranks "
	       "remove-empty "
	       "remove-first "
	       );


