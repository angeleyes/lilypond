/*
  time_signature.hh -- declare Time_signature

  (c) 1996--2005 Han-Wen Nienhuys
*/

#ifndef METER_HH
#define METER_HH

#include "item.hh"

/**
   Print a time_signature sign.

   TODO:

   2+3+2/8 time_signatures
  
 */
struct Time_signature
{
  static bool has_interface (Grob*);
  static Stencil special_time_signature (Grob*, SCM, int, int) ;
  static Stencil numbered_time_signature (Grob*, int, int);
  DECLARE_SCHEME_CALLBACK (print, (SCM ));
};
#endif // METER_HH

