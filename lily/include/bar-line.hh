/*
  bar.hh -- part of GNU LilyPond

  (c) 1996--2005 Han-Wen Nienhuys
*/

#ifndef BAR_HH
#define BAR_HH

#include "lily-guile.hh"
#include "lily-proto.hh"

/**
  A vertical bar.
 */
class Bar_line
{
public:
  static bool has_interface (Grob*);
  
  static Stencil compound_barline (Grob*, String, Real height) ;
  static Stencil simple_barline (Grob*, Real wid, Real height) ;      
  DECLARE_SCHEME_CALLBACK (get_staff_bar_size, (SCM ));
  DECLARE_SCHEME_CALLBACK (print, (SCM ));
  DECLARE_SCHEME_CALLBACK (before_line_breaking, (SCM ));
};
#endif // BAR_HH

