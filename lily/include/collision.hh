/*
  collision.hh -- declare Collision

  source file of the GNU LilyPond music typesetter

  (c) 1997 Han-Wen Nienhuys <hanwen@stack.nl>
*/


#ifndef COLLISION_HH
#define COLLISION_HH
#include "lily-proto.hh"
#include "item.hh"

/** TODO 

  multistaff support (see Chlapik: equal noteheads should be on the
  same hpos.)  
*/
class Collision : public Item {
protected:
    virtual void do_substitute_dependency(Score_elem*,Score_elem*);
    virtual void do_pre_processing();
public:
    Link_array<Note_column> clash_l_arr_;
    DECLARE_MY_RUNTIME_TYPEINFO;
    void add (Note_column*ncol_l);
    Collision();
    
};
#endif // COLLISION_HH
