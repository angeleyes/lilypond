/*
  grob-smob.cc -- implement GROB smob routines.

  source file of the GNU LilyPond music typesetter

  (c) 2005 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "grob.hh"

#include "paper-score.hh"

#include "ly-smobs.icc"

IMPLEMENT_SMOBS (Grob);
IMPLEMENT_DEFAULT_EQUAL_P (Grob);
IMPLEMENT_TYPE_P (Grob, "ly:grob?");

SCM
Grob::mark_smob (SCM ses)
{
  Grob *s = (Grob *) SCM_CELL_WORD_1 (ses);
  scm_gc_mark (s->immutable_property_alist_);
  scm_gc_mark (s->property_callbacks_);

  if (s->key_)
    scm_gc_mark (s->key_->self_scm ());
  for (int a = 0; a < 2; a++)
    {
      scm_gc_mark (s->dim_cache_[a].offset_callbacks_);
      scm_gc_mark (s->dim_cache_[a].dimension_);
      scm_gc_mark (s->dim_cache_[a].dimension_callback_);

      /* Do not mark the parents.  The pointers in the mutable
	 property list form two tree like structures (one for X
	 relations, one for Y relations).  Marking these can be done
	 in limited stack space.  If we add the parents, we will jump
	 between X and Y in an erratic manner, leading to much more
	 recursion depth (and core dumps if we link to pthreads).  */
    }

  if (s->original_)
    scm_gc_mark (s->original_->self_scm ());

  s->derived_mark ();
  scm_gc_mark (s->object_alist_);
  scm_gc_mark (s->interfaces_);

  return s->mutable_property_alist_;
}

int
Grob::print_smob (SCM s, SCM port, scm_print_state *)
{
  Grob *sc = (Grob *) SCM_CELL_WORD_1 (s);

  scm_puts ("#<Grob ", port);
  scm_puts ((char *) sc->name ().to_str0 (), port);

  /* Do not print properties, that is too much hassle.  */
  scm_puts (" >", port);
  return 1;
}

void
Grob::derived_mark () const
{
}
