/*   
  music-wrapper-iterator.hh -- declare Music_wrapper_iterator
  
  source file of the GNU LilyPond music typesetter
  
  (c)  1998--2003 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  
 */

#ifndef MUSIC_WRAPPER_ITERATOR_HH
#define MUSIC_WRAPPER_ITERATOR_HH

#include "music.hh"
#include "music-iterator.hh"

/** 
  The iterator for a #Music_wrapper#.  Since #Music_wrapper# essentially
  does nothing, this iterator creates a child iterator and delegates
  all work to that child.
 */
class Music_wrapper_iterator : public Music_iterator
{
public:
  VIRTUAL_COPY_CONS (Music_iterator);
  DECLARE_SCHEME_CALLBACK(constructor, ());  
  Music_wrapper_iterator ();
  Music_wrapper_iterator (Music_wrapper_iterator const&);

  virtual void derived_mark () const;
  virtual void construct_children () ;
  virtual Moment pending_moment () const;
  virtual void do_quit(); 
  virtual bool ok () const;
  virtual SCM get_pending_events (Moment)const;
  virtual void skip (Moment);
  virtual bool run_always () const;
protected:
  virtual void process (Moment);
  virtual Music_iterator *try_music_in_children (Music *) const;

  Music_iterator *child_iter_;
};

#endif /* MUSIC_WRAPPER_ITERATOR_HH */



