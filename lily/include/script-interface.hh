/*   
  script-interface.hh -- 

  source file of the GNU LilyPond music typesetter

  (c) 1999--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#ifndef SCRIPT_INTERFACE_HH
#define SCRIPT_INTERFACE_HH

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
  static Stencil get_stencil (Grob *, Direction d);
  static bool has_interface (Grob *);
  DECLARE_SCHEME_CALLBACK (print, (SCM ));
  DECLARE_SCHEME_CALLBACK (before_line_breaking, (SCM));
};

void make_script_from_event (Grob *p, SCM *descr, Context *tg,
			     SCM type, int index);

#endif /* SCRIPT_INTERFACE_HH */

