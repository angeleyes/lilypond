/*   
measure-grouping-spanner.hh -- declare 

source file of the GNU LilyPond music typesetter

(c) 2002--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>

 */

#ifndef MEASURE_GROUPING_SPANNER_HH
#define MEASURE_GROUPING_SPANNER_HH

#include "grob.hh"

class Measure_grouping
{
public:
  DECLARE_SCHEME_CALLBACK (print, (SCM ));
 
  static bool has_interface (Grob*);
};


#endif /* MEASURE_GROUPING_SPANNER_HH */

