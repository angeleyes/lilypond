/*
  change-iterator.hh -- declare Change_iterator

  source file of the GNU LilyPond music typesetter

  (c) 1997--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/


#ifndef CHANGE_ITERATOR_HH
#define CHANGE_ITERATOR_HH

#include "simple-music-iterator.hh"


class Change_iterator : public Simple_music_iterator
{
public:
  /* constructor is public */
  virtual void process (Moment);
  DECLARE_SCHEME_CALLBACK(constructor, ());

private:
  void  error (String);
};

#endif
