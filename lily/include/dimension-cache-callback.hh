/*   
  dimension-cache-callback.hh -- declare 
  
  source file of the GNU LilyPond music typesetter
  
  (c) 2000--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  
 */

#ifndef DIMENSION_CACHE_CALLBACK_HH
#define DIMENSION_CACHE_CALLBACK_HH

#include "axes.hh"

typedef Interval (*Dim_cache_callback) (Grob *, Axis);
typedef Real (*Offset_callback) (Grob *, Axis);

#endif /* DIMENSION_CACHE_CALLBACK_HH */

