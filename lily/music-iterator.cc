/*
  music-iterator.cc -- implement Music_iterator

  source file of the GNU LilyPond music typesetter

  (c) 1997--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

/*
  UGH. too many includes.
 */
#include <stdio.h>

#include "warn.hh"
#include "music-iterator.hh"
#include "context.hh"
#include "music-wrapper.hh"
#include "music-wrapper-iterator.hh"
#include "simple-music-iterator.hh"

#include "ly-smobs.icc"

Music_iterator::Music_iterator ()
{
  music_ = 0;
  smobify_self ();
}

Music_iterator::Music_iterator (Music_iterator const& )
{
  assert (false);
}

Music_iterator::~Music_iterator ()
{
}

Context * 
Music_iterator::get_outlet () const
{
  return handle_.get_outlet ();
}

void
Music_iterator::set_translator (Context *trans)
{
  handle_.set_translator (trans);
}

void
Music_iterator::construct_children ()
{
}

Moment
Music_iterator::pending_moment () const
{
  return 0;
}

void
Music_iterator::process (Moment)
{
}

bool
Music_iterator::ok () const
{
  return false;
}



SCM
Music_iterator::get_static_get_iterator (Music *m)
{
  Music_iterator * p =0;

  SCM ctor = m->get_property ("iterator-ctor") ;
  SCM iter = SCM_EOL;
  if (gh_procedure_p (ctor))
    {
      iter = gh_call0 (ctor);
      p = unsmob_iterator (iter);
    }
  else
    {
      if (dynamic_cast<Music_wrapper *> (m))
	p = new Music_wrapper_iterator;
      else
	p = new Simple_music_iterator;

      iter = p->self_scm();
      scm_gc_unprotect_object (iter);
    }

  p->music_ = m;
  assert (m);
  p->music_length_ = m->get_length ();
  p->start_mom_ = m->start_mom ();
  return iter;
}


Moment
Music_iterator::music_get_length () const
{
  return music_length_;
}

Moment
Music_iterator::music_start_mom ()const
{
  return start_mom_;
}

void
Music_iterator::init_translator (Music *m, Context *report)
{
  music_ = m;
  assert (m);
  if (! get_outlet ())
    set_translator (report);
}

void
Music_iterator::substitute_outlet (Context *f, Context *t)
{
  if (get_outlet () == f)
    set_translator (t);
  derived_substitute (f,t);
}

void
Music_iterator::derived_substitute (Context *,Context *)
{
}

SCM
Music_iterator::get_iterator (Music *m) const
{
  SCM ip = get_static_get_iterator (m);
  Music_iterator*p = unsmob_iterator (ip);
  
  p->init_translator (m, get_outlet ());
  
  p->construct_children ();
  return ip;
}

/*
  TODO: rename to prevent confusion between Translator::try_music and
  Iterator::try_music
  
 */
Music_iterator*
Music_iterator::try_music (Music *m) const
{
  bool b = get_outlet ()->try_music ((Music*)m); // ugh
  Music_iterator * it = b ? (Music_iterator*) this : 0;	// ugh
  if (!it)
    it = try_music_in_children (m);
  return it;
}

Music_iterator*
Music_iterator::try_music_in_children (Music *) const
{
  return 0;
}

IMPLEMENT_CTOR_CALLBACK (Music_iterator);

Music *
Music_iterator::get_music () const
{
  return music_;
}

/****************************************************************/

IMPLEMENT_TYPE_P (Music_iterator, "ly:iterator?");
IMPLEMENT_SMOBS(Music_iterator);
IMPLEMENT_DEFAULT_EQUAL_P(Music_iterator);

SCM
Music_iterator::mark_smob (SCM smob)
{
  Music_iterator * mus = (Music_iterator *)SCM_CELL_WORD_1 (smob);

  mus->derived_mark ();
  /*
    Careful with GC, although we intend the following as pointers
    only, we _must_ mark them.
   */
  if (mus->get_outlet ())
    scm_gc_mark (mus->get_outlet ()->self_scm());
  if (mus->music_)
    scm_gc_mark (mus->music_->self_scm());
  

  return SCM_EOL;
}

int
Music_iterator::print_smob (SCM sm , SCM port, scm_print_state*)
{
  char s[1000];

  Music_iterator *iter = unsmob_iterator (sm);
  sprintf (s, "#<%s>", classname (iter));
  scm_puts (s, port);
  return 1;
}

void
Music_iterator::derived_mark()const
{
}

void
Music_iterator::quit ()
{
  do_quit ();
  handle_.quit ();
}

void
Music_iterator::do_quit()
{
}

bool
Music_iterator::run_always ()const
{
  return false;
}
