/*   
  music-wrapper-iterator.cc --  implement Music_wrapper_iterator
  
  source file of the GNU LilyPond music typesetter
  
  (c) 1998--2003 Han-Wen Nienhuys <hanwen@cs.uu.nl>
 */


#include "music-wrapper-iterator.hh"
#include "music-wrapper.hh"

Music_wrapper_iterator::Music_wrapper_iterator ()
{
  child_iter_ =0;
}

Music_wrapper_iterator::Music_wrapper_iterator (Music_wrapper_iterator const &src)
  : Music_iterator (src)
{
  child_iter_ = 0;
  if  (src.child_iter_)
  {
    child_iter_ = src.child_iter_->clone () ;
    scm_gc_unprotect_object (child_iter_->self_scm());
  }
}

void
Music_wrapper_iterator::do_quit()
{
  child_iter_->quit();
}

void
Music_wrapper_iterator::derived_mark () const
{
  if (child_iter_)
    scm_gc_mark (child_iter_->self_scm());
}

void
Music_wrapper_iterator::derived_substitute (Translator_group*f,Translator_group*t) 
{
  
  if (child_iter_)
    child_iter_->substitute_outlet (f,t);
}

void
Music_wrapper_iterator::construct_children ()
{
  Music * m =  dynamic_cast<Music_wrapper const*> (get_music ())-> element();  
  child_iter_ = unsmob_iterator (get_iterator (m));
}

bool
Music_wrapper_iterator::ok () const
{
  return child_iter_ && child_iter_->ok ();
}


void
Music_wrapper_iterator::process (Moment m)
{
  child_iter_->process (m);
}


Moment
Music_wrapper_iterator::pending_moment () const
{
  return child_iter_->pending_moment ();
}

Music_iterator*
Music_wrapper_iterator::try_music_in_children (Music *m) const
{
  return child_iter_->try_music (m);
}

IMPLEMENT_CTOR_CALLBACK (Music_wrapper_iterator);

bool
Music_wrapper_iterator::run_always () const
{
  return child_iter_->run_always ();
}
