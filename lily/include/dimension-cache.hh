/*
  dimension-cache.hh -- declare Dimension_cache

  source file of the GNU LilyPond music typesetter

  (c) 1998--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#ifndef DIMENSION_CACHE_HH
#define DIMENSION_CACHE_HH

#include "lily-proto.hh"
#include "parray.hh"
#include "dimension-cache-callback.hh"
#include "lily-guile.hh"

/*
  XY offset/refpoint/extent structure.
*/
class Dimension_cache
{
  Interval *extent_;
  Real *offset_;
  Grob *parent_;
  
  friend class Grob;
  
  Dimension_cache (Dimension_cache const &);
  ~Dimension_cache ();
  Dimension_cache ();
  void init ();
};

#endif /* DIMENSION_CACHE_HH */

