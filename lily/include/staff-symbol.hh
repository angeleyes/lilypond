/*
  staff-symbol.hh -- declare Staff_symbol

  source file of the GNU LilyPond music typesetter

  (c)  1997--2003 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/


#ifndef STAFF_SYMBOL_HH
#define STAFF_SYMBOL_HH

#include "lily-guile.hh"
#include "lily-proto.hh"

/**
  TODO: add stafflinethickness as parameter.
  */
class Staff_symbol 
{
public:
  static Real staff_space (Grob*) ;
  static int get_steps (Grob*) ;
  static int line_count (Grob*);
  DECLARE_SCHEME_CALLBACK (brew_molecule, (SCM ));
  static bool has_interface (Grob*);
  
};
#endif // STAFF_SYMBOL_HH
