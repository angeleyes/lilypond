/*   
accidental-placement.hh -- declare  Accidental_placement

source file of the GNU LilyPond music typesetter

(c)  2002--2003 Han-Wen Nienhuys <hanwen@cs.uu.nl>

 */

#ifndef ACCIDENTAL_PLACEMENT_HH
#define ACCIDENTAL_PLACEMENT_HH

#include "grob.hh"

class Accidental_placement
{
public:
  DECLARE_SCHEME_CALLBACK (alignment_callback, (SCM element, SCM axis));
  static void add_accidental (Grob *,Grob* );

  static Interval get_relevant_accidental_extent (Grob *me,
						  Item *item_col,
						  Grob *acc);
  static void split_accidentals (Grob * accs,
			  Link_array<Grob> *break_reminder,
			  Link_array<Grob> *real_acc);
  
  static SCM position_accidentals (Grob* );
  static bool has_interface (Grob*);
};
#endif /* ACCIDENTAL_PLACEMENT_HH */

