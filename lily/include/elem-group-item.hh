/*
  elem-group-item.hh -- declare Horizontal_vertical_group_item
  
  source file of the GNU LilyPond music typesetter

  (c) 1997 Han-Wen Nienhuys <hanwen@stack.nl>
*/


#ifndef ELEM_GROUP_ITEM_HH
#define ELEM_GROUP_ITEM_HH

#include "axis-group-item.hh"
#include "elem-group.hh"


/**
  Treat a collection of items as a unity
 */
class Horizontal_vertical_group_item  : public Axis_group_item, public Horizontal_vertical_group_element {
protected:
  virtual void do_print() const;
  virtual void remove_all() { Horizontal_vertical_group_element::remove_all (); }
  virtual void do_unlink () { Horizontal_vertical_group_element::do_unlink (); }
  SCORE_ELEM_CLONE(Horizontal_vertical_group_item);
public:
  virtual void add_element (Graphical_element*e) { Horizontal_vertical_group_element::add_element (e); }
  virtual void remove_element (Graphical_element*e) { Horizontal_vertical_group_element::remove_element (e); }

  DECLARE_MY_RUNTIME_TYPEINFO;
};

#endif // ELEM_GROUP_ITEM_HH
