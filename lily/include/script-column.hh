/*
  script-column.hh -- declare Script_column

  source file of the GNU LilyPond music typesetter

  (c) 1997 Han-Wen Nienhuys <hanwen@stack.nl>
*/


#ifndef SCRIPT_COLUMN_HH
#define SCRIPT_COLUMN_HH

#include "elem-group.hh"
#include "item.hh"

/** a struct for treating a group of noteheads (noteheads, stem
  (chord) and scripts ) as a single entity.  */
class Script_column : public Item, public Element_group {

protected:
    virtual void do_print() const;
    virtual void do_substitute_dependency(Score_elem*, Score_elem*);
    virtual void do_pre_processing() ;
public:
    Link_array<Script> script_l_arr_;
    Link_array<Item> support_l_arr_;
    NAME_MEMBERS(Script_column);
   
    void add(Script *);
    void add_support(Item*);
};

#endif // SCRIPT_COLUMN_HH
