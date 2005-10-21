/*
  hara-kiri-vertical-group-spanner.cc -- implement Hara_kiri_group_spanner

  source file of the GNU LilyPond music typesetter

  (c) 1998--2005 Jan Nieuwenhuizen <janneke@gnu.org>
  Han-Wen Nienhuys <hanwen@cs.uu.nl>
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
Hara_kiri_group_spanner::y_extent (SCM element_smob)
{
  Grob *me = unsmob_grob (element_smob);
  consider_suicide (me);
  return Axis_group_interface::generic_group_extent (me, Y_AXIS);
}

void
Hara_kiri_group_spanner::consider_suicide (Grob *me)
{
  Spanner *sp = dynamic_cast<Spanner *> (me);

  extract_grob_set (me, "items-worth-living", worth);
  if (worth.size ())
    return;

  bool remove_first = to_boolean (me->get_property ("remove-first"));
  if (!remove_first
       && ((sp->original_ && broken_spanner_index (sp) == 0)
	   || Paper_column::get_rank (sp->get_bound (LEFT)->get_column ())
	   == 0)) 
    return;

  Link_array<Grob> childs;
  Axis_group_interface::get_children (me, &childs);
  for (int i = 0; i < childs.size (); i++)
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
MAKE_SCHEME_CALLBACK (Hara_kiri_group_spanner, force_hara_kiri_callback, 2);
SCM
Hara_kiri_group_spanner::force_hara_kiri_callback (SCM element_smob, SCM axis)
{
  Grob *me = unsmob_grob (element_smob);
  (void) axis;

  assert (scm_to_int (axis) == Y_AXIS);
  consider_suicide (me);
  return scm_from_double (0.0);
}

MAKE_SCHEME_CALLBACK (Hara_kiri_group_spanner, force_hara_kiri_in_parent_callback, 2);
SCM
Hara_kiri_group_spanner::force_hara_kiri_in_parent_callback (SCM element_smob, SCM axis)
{
  Grob *daughter = unsmob_grob (element_smob);
  Axis a = (Axis) scm_to_int (axis);
  assert (a == Y_AXIS);
  force_hara_kiri_callback (daughter->get_parent (a)->self_scm (), axis);
  return scm_from_double (0.0);
}

void
Hara_kiri_group_spanner::add_element (Grob *me, Grob *e)
{
  //  e->add_offset_callback (force_hara_kiri_in_parent_callback, Y_AXIS);
  Axis_group_interface::add_element (me, e);
}

void
Hara_kiri_group_spanner::add_interesting_item (Grob *me, Grob *n)
{
  Pointer_group_interface::add_grob (me, ly_symbol2scm ("items-worth-living"), n);
}

ADD_INTERFACE (Hara_kiri_group_spanner, "hara-kiri-group-interface",
	       "A group spanner that  keeps track of interesting items.  If it "
	       "doesn't contain any after linebreaking, then it "
	       "will remove itself and all its children.",


	       /* properties */
	       "items-worth-living "
	       "remove-first");

