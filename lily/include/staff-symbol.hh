/*
  staff-symbol.hh -- declare Staff_symbol

  source file of the GNU LilyPond music typesetter

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#ifndef STAFF_SYMBOL_HH
#define STAFF_SYMBOL_HH

#include "lily-guile.hh"
#include "lily-proto.hh"

/**
   TODO: add linethickness as parameter.
*/
class Staff_symbol
{
public:
  static Real staff_space (Grob *);
  static Real get_line_thickness (Grob *);
  static Real get_ledger_line_thickness (Grob *);

  static int get_steps (Grob *);
  static int line_count (Grob *);
  DECLARE_SCHEME_CALLBACK (print, (SCM));
  static bool has_interface (Grob *);

};
#endif // STAFF_SYMBOL_HH
