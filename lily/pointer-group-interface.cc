/*
  pointer-group-interface.cc -- implement Pointer_group_interface

  source file of the GNU LilyPond music typesetter

  (c) 2005 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "pointer-group-interface.hh"

#include "lily-proto.hh"
#include "item.hh"
#include "grob-array.hh"
#include "grob.hh"

int
Pointer_group_interface::count (Grob *me, SCM sym)
{
  Grob_array *arr = unsmob_grob_array (me->internal_get_object (sym));
  return arr ? arr->size () : 0;
}

void
Pointer_group_interface::add_grob (Grob *me, SCM sym, SCM p)
{
  add_grob (me, sym, unsmob_grob (p));
}

void
Pointer_group_interface::add_grob (Grob *me, SCM sym, Grob *p)
{
  SCM scm_arr = me->internal_get_object (sym);
  Grob_array *arr = unsmob_grob_array (scm_arr);
  if (!arr)
    {
      scm_arr = Grob_array::make_array ();
      arr = unsmob_grob_array (scm_arr);
      me->internal_set_object (sym, scm_arr);
    }
  arr->add (p);
}

static Link_array<Grob> empty_array;

Link_array<Grob> const &
ly_scm2link_array (SCM x)
{
  Grob_array *arr = unsmob_grob_array (x);
  return arr ? arr->array () : empty_array;
}

Link_array<Grob> const &
internal_extract_grob_array (Grob const *elt, SCM symbol)
{
  return ly_scm2link_array (elt->internal_get_object (symbol));
}

Link_array<Item>
internal_extract_item_array (Grob const *elt, SCM symbol)
{
  Grob_array *arr = unsmob_grob_array (elt->internal_get_object (symbol));
  Link_array<Item> items;
  for (int i = 0; arr && i < arr->size (); i++)
    items.push (arr->item (i));

  return items;
}
