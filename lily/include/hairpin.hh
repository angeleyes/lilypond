/*
  hairpin.hh -- declare Hairpin

  source file of the GNU LilyPond music typesetter

  (c)  1997--2003 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/


#ifndef HAIRPIN_HH
#define HAIRPIN_HH

#include "lily-guile.hh"
#include "lily-proto.hh"
/**
  The hairpin symbol. 
 */
struct Hairpin
{
public:
  DECLARE_SCHEME_CALLBACK (brew_molecule, (SCM));
  static bool has_interface (Grob*);
};

#endif // HAIRPIN_HH
