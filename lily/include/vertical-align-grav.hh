/*
  vertical-align-grav.hh -- declare Vertical_align_engraver

  source file of the GNU LilyPond music typesetter

  (c) 1997 Han-Wen Nienhuys <hanwen@stack.nl>
*/


#ifndef VERTICAL_ALIGN_GRAV_HH
#define VERTICAL_ALIGN_GRAV_HH

#include "engraver.hh"

class Vertical_align_engraver : public Engraver {
  Vertical_align_spanner * valign_p_;
    
public:
  TRANSLATOR_CLONE(Vertical_align_engraver);
  DECLARE_MY_RUNTIME_TYPEINFO;
  Vertical_align_engraver();
protected:
  virtual void acknowledge_element (Score_elem_info);
  virtual void do_creation_processing();
  virtual void do_removal_processing();
};

#endif // VERTICAL_ALIGN_GRAV_HH
