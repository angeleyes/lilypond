/*
  simultaneous-music-iterator.hh -- declare Simultaneous_music_iterator

  source file of the GNU LilyPond music typesetter

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/


#ifndef SIMULTANEOUS_MUSIC_ITERATOR_HH
#define SIMULTANEOUS_MUSIC_ITERATOR_HH

#include "music-iterator.hh"

class Simultaneous_music_iterator : public Music_iterator
{
public:
  Simultaneous_music_iterator ();
  Simultaneous_music_iterator (Simultaneous_music_iterator const&);
  virtual void derived_substitute (Context *f, Context *t) ;
  virtual void derived_mark () const;
  DECLARE_SCHEME_CALLBACK(constructor, ());
  
  /// make a new context for every child.
  bool create_separate_contexts_;

  virtual void construct_children ();
  virtual Moment pending_moment () const;
  virtual void do_quit(); 
  virtual bool ok () const;
  virtual bool run_always () const;
  
protected:
  virtual void process (Moment);
  virtual Music_iterator *try_music_in_children (Music *) const;

private:
  SCM children_list_;
};

#endif // SIMULTANEOUS_MUSIC_ITERATOR_HH
