/*
  property-iterator.hh -- declare Property_iterator

  source file of the GNU LilyPond music typesetter

  (c)  1997--1998 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/


#ifndef PROPERTY_ITERATOR_HH
#define PROPERTY_ITERATOR_HH

#include "music-iterator.hh"
/**
  Iterate a property.  
 */
class Property_iterator : public Music_iterator
{
public:
  // construction  
protected:
  virtual void do_process_and_next (Moment);
};

#endif // PROPERTY_ITERATOR_HH
