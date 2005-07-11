/*
  item.cc -- implement Item

  source file of the GNU LilyPond music typesetter

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "item.hh"

#include "paper-score.hh"
#include "warn.hh"
#include "paper-column.hh"
#include "lily-guile.hh"
#include "system.hh"
#include "group-interface.hh"

Grob *
Item::clone (int count) const
{
  return new Item (*this, count);
}

Item::Item (SCM s, Object_key const *key)
  : Grob (s, key)
{
  broken_to_drul_[LEFT] = broken_to_drul_[RIGHT] = 0;
  Group_interface::add_thing (this, ly_symbol2scm ("interfaces"), ly_symbol2scm ("item-interface"));
}

/**
   Item copy ctor.  Copy nothing: everything should be a elt property
   or a special purpose pointer (such as broken_to_drul_[]) */
Item::Item (Item const &s, int copy_count)
  : Grob (s, copy_count)
{
  broken_to_drul_[LEFT] = broken_to_drul_[RIGHT] = 0;
}

bool
Item::is_breakable (Grob *me)
{
  if (me->original_)
    return false;

  if (!dynamic_cast<Item *> (me))
    me->programming_error ("only items can be breakable.");

  Item *i = dynamic_cast<Item *> (me->get_parent (X_AXIS));
  return (i) ? Item::is_breakable (i) : to_boolean (me->get_property ("breakable"));
}

Paper_column *
Item::get_column () const
{
  Item *parent = dynamic_cast<Item *> (get_parent (X_AXIS));
  return parent ? parent->get_column () : 0;
}

System *
Item::get_system () const
{
  Grob *g = get_parent (X_AXIS);
  return g ? g->get_system () : 0;
}

void
Item::copy_breakable_items ()
{
  Drul_array<Item *> new_copies;
  Direction i = LEFT;
  int count = 0;
  do
    {
      Grob *dolly = clone (count++);
      Item *item = dynamic_cast<Item *> (dolly);
      pscore_->root_system ()->typeset_grob (item);
      new_copies[i] = item;
    }
  while (flip (&i) != LEFT);

  broken_to_drul_ = new_copies;
}

bool
Item::is_broken () const
{
  return broken_to_drul_[LEFT] || broken_to_drul_[RIGHT];
}

/*
  Generate items for begin and end-of line.
*/
void
Item::discretionary_processing ()
{
  if (is_broken ())
    return;

  if (Item::is_breakable (this))
    copy_breakable_items ();
}

Grob *
Item::find_broken_piece (System *l) const
{
  if (get_system () == l)
    return (Item *) (this);

  Direction d = LEFT;
  do
    {
      Grob *s = broken_to_drul_[d];
      if (s && s->get_system () == l)
	return s;
    }
  while (flip (&d) != LEFT);

  return 0;
}

Item *
Item::find_prebroken_piece (Direction d) const
{
  Item *me = (Item *) (this);
  if (!d)
    return me;
  return dynamic_cast<Item *> (broken_to_drul_[d]);
}

Direction
Item::break_status_dir () const
{
  if (original_)
    {
      Item *i = dynamic_cast<Item *> (original_);

      return (i->broken_to_drul_[LEFT] == this) ? LEFT : RIGHT;
    }
  else
    return CENTER;
}

void
Item::handle_prebroken_dependencies ()
{
  Grob::handle_prebroken_dependencies ();

  /*
    Can't do this earlier, because try_visibility_lambda () might set
    the elt property transparent, which would then be copied.

    TODO:

    give the item to break-visibility itself, so the function can do
    more complicated things.
  */
  SCM vis = get_property ("break-visibility");
  if (ly_is_procedure (vis))
    {
      SCM args = scm_list_n (scm_int2num (break_status_dir ()), SCM_UNDEFINED);
      SCM result = scm_apply_0 (vis, args);
      bool trans = ly_scm2bool (scm_car (result));
      bool empty = ly_scm2bool (scm_cdr (result));

      if (empty && trans)
	suicide ();
      else if (empty)
	{
	  set_extent (SCM_EOL, X_AXIS);
	  set_extent (SCM_EOL, Y_AXIS);
	}
      else if (trans)
	set_property ("print-function", SCM_EOL);
    }
}

SCM
Item::do_derived_mark () const
{
  if (broken_to_drul_[LEFT])
    scm_gc_mark (broken_to_drul_[LEFT]->self_scm ());
  if (broken_to_drul_[RIGHT])
    scm_gc_mark (broken_to_drul_[RIGHT]->self_scm ());
  return SCM_EOL;
}

Item *
unsmob_item (SCM s)
{
  return dynamic_cast<Item *> (unsmob_grob (s));
}

ADD_INTERFACE (Item,
	       "item-interface",
	       "\n"
	       "\n"
	       "Grobs can be distinguished in their role in the horizontal spacing.\n"
	       "Many grobs define constraints on the spacing by their sizes. For\n"
	       "example, note heads, clefs, stems, and all other symbols with a fixed\n"
	       "shape.  These grobs form a subtype called @code{Item}.\n"
	       "\n"
	       "\n"
	       "Some items need special treatment for line breaking. For example, a\n"
	       "clef is normally only printed at the start of a line (i.e. after a\n"
	       "line break).  To model this, `breakable' items (clef, key signature,\n"
	       "bar lines, etc.) are copied twice. Then we have three versions of each\n"
	       "breakable item: one version if there is no line break, one version\n"
	       "that is printed before the line break (at the end of a system), one\n"
	       "version that is printed after the line break.\n"
	       "\n"
	       "Whether these versions are visible and take up space, is determined by\n"
	       "the outcome of the @code{break-visibility}. This grob property is a\n"
	       "function taking a direction (-1, 0 or 1) as argument. It returns a\n"
	       "cons of booleans, signifying whether this grob should be transparent\n"
	       "and have no extent.\n"
	       "\n"
	       "The following variables for break-visibility are predefined:\n"
	       "@example\n"
	       "           grob will show:   before  no     after\n"
	       "                             break   break  break\n"
	       "  all-invisible              no      no     no\n"
	       "  begin-of-line-visible      no      no     yes\n"
	       "  end-of-line-visible        yes     no     no\n"
	       "  all-visible                yes     yes    yes\n"
	       "  begin-of-line-invisible    yes     yes    no\n"
	       "  end-of-line-invisible      no      yes    yes\n"
	       "@end example\n",
	       "no-spacing-rods break-visibility breakable")
