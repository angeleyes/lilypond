/*
  spacing-interface.cc -- functionality that is shared between Note_spacing
  and Staff_spacing

  source file of the GNU LilyPond music typesetter

  (c) 2007 Joe Neeman <joeneeman@gmail.com>
*/

#include "spacing-interface.hh"

#include "grob.hh"
#include "grob-array.hh"
#include "item.hh"
#include "note-column.hh"
#include "pointer-group-interface.hh"
#include "paper-column.hh"
#include "separation-item.hh"
#include "skyline.hh"

/* return the right-pointing skyline of the left-items and the left-pointing
   skyline of the right-items (with the skyline of the left-items in
   ret[LEFT]) */
Drul_array<Skyline>
Spacing_interface::skylines (Grob *me, Grob *right_col)
{
  /* the logic here is a little convoluted.
     A {Staff,Note}_spacing doesn't copy left-items when it clones,
     so in order to find the separation items, we need to use the original
     spacing grob. But once we find the separation items, we need to get back
     the broken piece.
  */

  Grob *orig = me->original () ? me->original () : me;
  Drul_array<Direction> break_dirs (dynamic_cast<Item*> (me)->break_status_dir (),
				    dynamic_cast<Item*> (right_col)->break_status_dir ());
  Drul_array<Skyline> skylines = Drul_array<Skyline> (Skyline (RIGHT), Skyline (LEFT));
  Drul_array<vector<Grob*> > items (ly_scm2link_array (orig->get_object ("left-items")),
				    ly_scm2link_array (orig->get_object ("right-items")));

  Direction d = LEFT;
  do
    {
      skylines[d].set_minimum_height (0.0);

      for (vsize i = 0; i < items[d].size (); i++)
	{
	  Grob *g = items[d][i];
	  if (Item *it = dynamic_cast<Item*> (g))
	    if (Grob *piece = it->find_prebroken_piece (break_dirs[d]))
	      g = piece;

	  if (Separation_item::has_interface (g))
	    {
	      SCM sky_scm = g->get_property ("horizontal-skylines");
	      Skyline_pair *sky = Skyline_pair::unsmob (sky_scm);
	      if (sky)
		skylines[d].merge ((*sky)[-d]);
	      else
		programming_error ("separation item has no skyline");
	    
	      if (d == RIGHT && items[LEFT].size ())
		skylines[d].merge (Separation_item::conditional_skyline (items[d][i], items[LEFT][0]));
	    }
	}
    }
  while (flip (&d) != LEFT);

  return skylines;
}

Real
Spacing_interface::minimum_distance (Grob *me, Grob *right)
{
  Drul_array<Skyline> skylines = Spacing_interface::skylines (me, right);

  return max (0.0, skylines[LEFT].distance (skylines[RIGHT]));
}

/*
  Compute the column of the right-items.  This is a big function,
  since RIGHT-ITEMS may span more columns (eg. if a clef is inserted,
  this will add a new column to RIGHT-ITEMS. Here we look at the
  columns, and return the left-most. If there are multiple columns, we
  prune RIGHT-ITEMS.
*/
Item *
Spacing_interface::right_column (Grob *me)
{
  if (!me->is_live ())
    return 0;

  Grob_array *a = unsmob_grob_array (me->get_object ("right-items"));
  Item *mincol = 0;
  int min_rank = INT_MAX;
  bool prune = false;
  for (vsize i = 0; a && i < a->size (); i++)
    {
      Item *ri = a->item (i);
      Item *col = ri->get_column ();

      int rank = Paper_column::get_rank (col);

      if (rank < min_rank)
	{
	  min_rank = rank;
	  if (mincol)
	    prune = true;

	  mincol = col;
	}
    }

  if (prune && a)
    {
      vector<Grob*> &right = a->array_reference ();
      for (vsize i = right.size (); i--;)
	{
	  if (dynamic_cast<Item *> (right[i])->get_column () != mincol)
	    right.erase (right.begin () + i);
	}
    }

  return mincol;
}

Item *
Spacing_interface::left_column (Grob *me)
{
  if (!me->is_live ())
    return 0;

  return dynamic_cast<Item *> (me)->get_column ();
}

static vector<Item*>
get_note_columns (vector<Grob*> const &elts)
{
  vector<Item*> ret;

  for (vsize i = 0; i < elts.size (); i++)
    {
      if (Note_column::has_interface (elts[i]))
	ret.push_back (dynamic_cast<Item*> (elts[i]));
      else if (Separation_item::has_interface (elts[i]))
	{
	  extract_grob_set (elts[i], "elements", more_elts);
	  vector<Item*> ncs = get_note_columns (more_elts);

	  ret.insert (ret.end (), ncs.begin (), ncs.end ());
	}
    }

  return ret;
}

vector<Item*>
Spacing_interface::right_note_columns (Grob *me)
{
  extract_grob_set (me, "right-items", elts);
  return get_note_columns (elts);
}

vector<Item*>
Spacing_interface::left_note_columns (Grob *me)
{
  extract_grob_set (me, "left-items", elts);
  return get_note_columns (elts);
}

/*
  Try to find the break-aligned symbol that belongs on the D-side
  of ME, sticking out in direction -D. The x size is put in LAST_EXT
*/
Grob *
Spacing_interface::extremal_break_aligned_grob (Grob *me,
						Direction d,
						Direction break_dir,
						Interval *last_ext)
{
  Grob *col = 0;
  last_ext->set_empty ();
  Grob *last_grob = 0;

  extract_grob_set (me, d == LEFT ? "left-break-aligned" : "right-break-aligned", elts);

  for (vsize i = elts.size (); i--;)
    {
      Item *break_item = dynamic_cast<Item*> (elts[i]);

      if (break_item->break_status_dir () != break_dir)
	break_item = break_item->find_prebroken_piece (break_dir);

      if (!break_item || !scm_is_pair (break_item->get_property ("space-alist")))
	continue;

      if (!col)
	col = dynamic_cast<Item*> (elts[0])->get_column ()->find_prebroken_piece (break_dir);

      Interval ext = break_item->extent (col, X_AXIS);

      if (ext.is_empty ())
	continue;

      if (!last_grob
	  || (last_grob && d * (ext[-d]- (*last_ext)[-d]) < 0))
	{
	  *last_ext = ext;
	  last_grob = break_item;
	}
    }

  return last_grob;
}


ADD_INTERFACE (Spacing_interface,
	       "This object calculates the desired and minimum distances between two columns.",

	       "left-items "
	       "right-items "
	       );