/*
  score-align-grav.hh -- declare Type_align_engraver

  source file of the GNU LilyPond music typesetter

  (c) 1997 Han-Wen Nienhuys <hanwen@stack.nl>
*/


#ifndef SCOREF_ALIGN_GRAV_HH
#define SCOREF_ALIGN_GRAV_HH

#include "engraver.hh"
#include "assoc.hh"

/**
  Group  a number of items across staffs
 */
class Score_priority_engraver : public Engraver
{
  Assoc<int, Horizontal_group_item  *> align_p_assoc_;
public:
  TRANSLATOR_CLONE(Score_priority_engraver);
  Score_priority_engraver ();
  DECLARE_MY_RUNTIME_TYPEINFO;
protected:
  virtual void acknowledge_element (Score_elem_info);
  virtual void do_pre_move_processing();
};

#endif // SCORE_ALIGN_GRAV_HH
