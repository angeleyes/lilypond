/*   
  script.hh -- declare Script
  
  source file of the GNU LilyPond music typesetter
  
  (c) 1999--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  
 */

#ifndef SCRIPT_HH
#define SCRIPT_HH

#include "lily-guile.hh"
#include "lily-proto.hh"

/**
   Articulation marks (and the like) that are attached to notes/stems.
   Needs support from Staff_side for proper operation.  Staff_side
   handles the positioning.
*/
class Script_interface
{
public:
  static Molecule get_molecule (Grob*,Direction d);
  
  static bool  has_interface (Grob*);
  DECLARE_SCHEME_CALLBACK (brew_molecule, (SCM ));
  DECLARE_SCHEME_CALLBACK (before_line_breaking, (SCM ));
};

void make_script_from_event (Grob *p,
			     SCM * descr, Translator_group*tg,
			     SCM type, 
			     int index);
#endif /* Stem_SCRIPT_HH */

