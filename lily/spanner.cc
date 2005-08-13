/*
  spanner.cc -- implement Spanner

  source file of the GNU LilyPond music typesetter

  (c) 1996--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "spanner.hh"

#include <math.h>

#include "pointer-group-interface.hh"
#include "libc-extension.hh"
#include "paper-column.hh"
#include "paper-column.hh"
#include "paper-score.hh"
#include "stencil.hh"
#include "system.hh"
#include "warn.hh"

Grob *
Spanner::clone (int count) const
{
  return new Spanner (*this, count);
}

void
Spanner::do_break_processing ()
{
  //break_into_pieces
  Item *left = spanned_drul_[LEFT];
  Item *right = spanned_drul_[RIGHT];

  if (!left || !right)
    return;

  /*
    Check if our parent in X-direction spans equally wide
    or wider than we do.
  */
  for (int a = X_AXIS; a < NO_AXES; a++)
    {
      if (Spanner *parent = dynamic_cast<Spanner *> (get_parent ((Axis)a)))
	{
	  if (!parent->spanned_rank_iv ().superset (this->spanned_rank_iv ()))
	    {
	      programming_error (to_string ("Spanner `%s' is not fully contained in parent spanner `%s'.",
					    name ().to_str0 (),
					    parent->name ().to_str0 ()));
	    }
	}
    }

  if (get_system () || is_broken ())
    return;

  if (left == right)
    {
      /*
	If we have a spanner spanning one column, we must break it
	anyway because it might provide a parent for another item.  */
      Direction d = LEFT;
      do
	{
	  Item *bound = left->find_prebroken_piece (d);
	  if (!bound)
	    programming_error ("no broken bound");
	  else if (bound->get_system ())
	    {
	      Spanner *span = dynamic_cast<Spanner *> (clone (broken_intos_.size ()));
	      span->set_bound (LEFT, bound);
	      span->set_bound (RIGHT, bound);

	      assert (span->get_system ());
	      span->get_system ()->typeset_grob (span);
	      broken_intos_.push (span);
	    }
	}
      while ((flip (&d)) != LEFT);
    }
  else
    {
      Link_array<Item> break_points = pscore_->root_system ()->broken_col_range (left, right);

      break_points.insert (left, 0);
      break_points.push (right);

      for (int i = 1; i < break_points.size (); i++)
	{
	  Drul_array<Item *> bounds;
	  bounds[LEFT] = break_points[i - 1];
	  bounds[RIGHT] = break_points[i];
	  Direction d = LEFT;
	  do
	    {
	      if (!bounds[d]->get_system ())
		bounds[d] = bounds[d]->find_prebroken_piece (- d);
	    }
	  while ((flip (&d)) != LEFT);

	  if (!bounds[LEFT] || ! bounds[RIGHT])
	    {
	      programming_error ("bounds of this piece aren't breakable. ");
	      continue;
	    }

	  Spanner *span = dynamic_cast<Spanner *> (clone (broken_intos_.size ()));
	  span->set_bound (LEFT, bounds[LEFT]);
	  span->set_bound (RIGHT, bounds[RIGHT]);

	  if (!bounds[LEFT]->get_system ()

	      || !bounds[RIGHT]->get_system ()
	      || bounds[LEFT]->get_system () != bounds[RIGHT]->get_system ())
	    {
	      programming_error ("bounds of spanner are invalid");
	      span->suicide ();
	    }
	  else
	    {
	      bounds[LEFT]->get_system ()->typeset_grob (span);
	      broken_intos_.push (span);
	    }
	}
    }
  broken_intos_.sort (Spanner::compare);
  for (int i = broken_intos_.size ();i--;)
    broken_intos_[i]->break_index_ = i;
}

int
Spanner::get_break_index () const
{
  return break_index_;
}

void
Spanner::set_my_columns ()
{
  Direction i = (Direction) LEFT;
  do
    {
      if (!spanned_drul_[i]->get_system ())
	set_bound (i, spanned_drul_[i]->find_prebroken_piece ((Direction) -i));
    }
  while (flip (&i) != LEFT);
}

Interval_t<int>
Spanner::spanned_rank_iv ()
{
  Interval_t<int> iv (0, 0);

  if (spanned_drul_[LEFT] && spanned_drul_[LEFT]->get_column ())
    iv[LEFT] = spanned_drul_[LEFT]->get_column ()->get_rank ();
  if (spanned_drul_[RIGHT] && spanned_drul_[RIGHT]->get_column ())
    iv[RIGHT] = spanned_drul_[RIGHT]->get_column ()->get_rank ();
  return iv;
}

Item *
Spanner::get_bound (Direction d) const
{
  return spanned_drul_ [d];
}

/*
  Set the items that this spanner spans. If D == LEFT, we also set the
  X-axis parent of THIS to S.
*/
void
Spanner::set_bound (Direction d, Grob *s)
{
  Item *i = dynamic_cast<Item *> (s);
  if (!i)
    {
      programming_error ("must have Item for spanner bound");
      return;
    }

  spanned_drul_[d] = i;

  /**
     We check for System to prevent the column -> line_of_score
     -> column -> line_of_score -> etc situation */
  if (d == LEFT && !dynamic_cast<System *> (this))
    set_parent (i, X_AXIS);

  /*
    Signal that this column needs to be kept alive. They need to be
    kept alive to have meaningful position and linebreaking.

    [maybe we should try keeping all columns alive?, and perhaps
    inherit position from their (non-)musical brother]

  */
  if (dynamic_cast<Paper_column *> (i))
    Pointer_group_interface::add_grob (i, ly_symbol2scm ("bounded-by-me"), this);
}

Spanner::Spanner (SCM s, Object_key const *key)
  : Grob (s, key)
{
  break_index_ = 0;
  spanned_drul_[LEFT] = 0;
  spanned_drul_[RIGHT] = 0;
}

Spanner::Spanner (Spanner const &s, int count)
  : Grob (s, count)
{
  spanned_drul_[LEFT] = spanned_drul_[RIGHT] = 0;
}

Real
Spanner::spanner_length () const
{
  Real l = spanned_drul_[LEFT]->relative_coordinate (0, X_AXIS);
  Real r = spanned_drul_[RIGHT]->relative_coordinate (0, X_AXIS);

  if (r < l)
    programming_error ("spanner with negative length");

  return r - l;
}

System *
Spanner::get_system () const
{
  if (!spanned_drul_[LEFT] || !spanned_drul_[RIGHT])
    return 0;
  if (spanned_drul_[LEFT]->get_system () != spanned_drul_[RIGHT]->get_system ())
    return 0;
  return spanned_drul_[LEFT]->get_system ();
}

Grob *
Spanner::find_broken_piece (System *l) const
{
  int idx = binsearch_links (broken_intos_, (Spanner *)l, Spanner::compare);

  if (idx < 0)
    return 0;
  else
    return broken_intos_ [idx];
}

int
Spanner::compare (Spanner *const &p1, Spanner *const &p2)
{
  return p1->get_system ()->get_rank () - p2->get_system ()->get_rank ();
}

bool
Spanner::is_broken () const
{
  return broken_intos_.size ();
}

/*
  If this is a broken spanner, return the amount the left end is to be
  shifted horizontally so that the spanner starts after the initial
  clef and key on the staves. This is necessary for ties, slurs,
  crescendo and decrescendo signs, for example.
*/
Real
Spanner::get_broken_left_end_align () const
{
  Paper_column *sc = dynamic_cast<Paper_column *> (spanned_drul_[LEFT]->get_column ());

  // Relevant only if left span point is first column in line
  if (sc != NULL
      && sc->break_status_dir () == RIGHT)
    {
      /*
	We used to do a full search for the Break_align_item.
	But that doesn't make a difference, since the Paper_column
	is likely to contain only a Break_align_item.
      */
      return sc->extent (sc, X_AXIS)[RIGHT];
    }

  return 0.0;
}

void
Spanner::derived_mark () const
{
  Direction d = LEFT;
  do
    if (spanned_drul_[d])
      scm_gc_mark (spanned_drul_[d]->self_scm ());
  while (flip (&d) != LEFT)
    ;

  for (int i = broken_intos_.size (); i--;)
    scm_gc_mark (broken_intos_[i]->self_scm ());
}

/*
  Set left or right bound to IT.

  Warning: caller should ensure that subsequent calls put in ITems
  that are left-to-right ordered.
*/
void
add_bound_item (Spanner *sp, Grob *it)
{
  if (!sp->get_bound (LEFT))
    sp->set_bound (LEFT, it);
  else
    sp->set_bound (RIGHT, it);
}

MAKE_SCHEME_CALLBACK (Spanner, set_spacing_rods, 1);
SCM
Spanner::set_spacing_rods (SCM smob)
{
  Grob *me = unsmob_grob (smob);

  Rod r;
  Spanner *sp = dynamic_cast<Spanner *> (me);
  r.item_drul_[LEFT] = sp->get_bound (LEFT);
  r.item_drul_[RIGHT] = sp->get_bound (RIGHT);
  r.distance_
    = robust_scm2double (me->get_property ("minimum-length"), 0);

  r.add_to_cols ();
  return SCM_UNSPECIFIED;
}

/*
  Return I such that SP == SP->ORIGINAL_->BROKEN_INTOS_[I].
*/
int
broken_spanner_index (Spanner *sp)
{
  Spanner *parent = dynamic_cast<Spanner *> (sp->original_);
  return parent->broken_intos_.find_index (sp);
}

Spanner *
unsmob_spanner (SCM s)
{
  return dynamic_cast<Spanner *> (unsmob_grob (s));
}

ADD_INTERFACE (Spanner,
	       "spanner-interface",
	       "Some objects are horizontally spanned between objects. For\n"
	       "example, slur, beam, tie, etc. These grobs form a subtype called\n"
	       "@code{Spanner}. All spanners have two span-points (these must be\n"
	       "@code{Item} objects), one on the left and one on the right. The left bound is\n"
	       "also the X-reference point of the spanner.\n",
	       "minimum-length");
