/*
  Sequential_music-iterator.hh -- declare Sequential_music_iterator

  source file of the GNU LilyPond music typesetter

  (c)  1997--2003 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#ifndef SEQUENTIAL_MUSIC_ITERATOR_HH
#define SEQUENTIAL_MUSIC_ITERATOR_HH

#include "sequential-iterator.hh"

/** Sequential_music iteration: walk each element in turn, and
  construct an iterator for every element.
  
 */
class Sequential_music_iterator :  public Sequential_iterator
{
public:
  DECLARE_SCHEME_CALLBACK(constructor, ());
private:
  virtual SCM get_music_list()const;
};

#endif // SEQUENTIAL_MUSIC_ITERATOR_HH
