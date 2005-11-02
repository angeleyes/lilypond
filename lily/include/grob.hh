/*
  grob.hh -- declare Grob

  source file of the LilyPond music typesetter

  (c) 1996--2005 Han-Wen Nienhuys
*/

#ifndef GROB_HH
#define GROB_HH

#include "virtual-methods.hh"
#include "dimension-cache.hh"
#include "grob-interface.hh"
#include "object-key.hh"

typedef void (Grob:: *Grob_method_pointer) (void);

class Grob
{
private:
  DECLARE_SMOBS (Grob, foo);
  void init ();

protected:
  Object_key const *key_;
  SCM immutable_property_alist_;
  SCM mutable_property_alist_;
  SCM object_alist_;
  
  /*
    If this is a property, it accounts for 25% of the property
    lookups.
  */
  SCM interfaces_;

  /* BARF */
  friend class Spanner;
  friend SCM ly_grob_properties (SCM);
  friend SCM ly_grob_basic_properties (SCM);
  void substitute_object_links (SCM, SCM);

  DECLARE_CLASSNAME(Grob);
  Real get_offset (Axis a) const;

  Output_def *layout_;
  Dimension_cache dim_cache_[NO_AXES];
  Grob *original_;

  SCM try_callback (SCM, SCM);
public:
  DECLARE_SCHEME_CALLBACK(x_parent_positioning, (SCM));
  DECLARE_SCHEME_CALLBACK(y_parent_positioning, (SCM));
  DECLARE_SCHEME_CALLBACK (stencil_height, (SCM smob));
  DECLARE_SCHEME_CALLBACK (stencil_width, (SCM smob));

  /* R/O access */
  Output_def *layout () const { return layout_; }
  Grob *original () const { return original_; }
  SCM interfaces () const { return interfaces_; }
  Object_key const *key () const { return key_; }

  /* life & death */ 
  Grob (SCM basic_props, Object_key const *);
  Grob (Grob const &, int copy_count);
  virtual Grob *clone (int count) const;

  /* forced death */
  void suicide ();
  bool is_live () const;

  /* naming. */
  String name () const;

  /* Properties */
  SCM get_property_alist_chain (SCM) const;
  SCM internal_get_property (SCM symbol) const;
  SCM get_property_data (SCM symbol) const;
  SCM internal_get_object (SCM symbol) const;
  void del_property (SCM symbol); 
  void internal_set_property (SCM sym, SCM val);
  void internal_set_object (SCM sym, SCM val);

  /* messages */  
  void warning (String) const;
  void programming_error (String) const;


  /* class hierarchy */
  virtual System *get_system () const;
  virtual void do_break_processing ();
  virtual Grob *find_broken_piece (System *) const;
  virtual void discretionary_processing ();
  virtual void derived_mark () const;
  virtual void handle_broken_dependencies ();
  virtual void handle_prebroken_dependencies ();

  /* printing */
  Stencil *get_stencil () const;
  Stencil get_print_stencil () const;

  /* interfaces */
  bool internal_has_interface (SCM intf);
  static bool has_interface (Grob *me);

  /* offsets */
  void translate_axis (Real, Axis);
  Real relative_coordinate (Grob const *refp, Axis) const;

  /* extents */
  Interval extent (Grob *refpoint, Axis) const;
  void flush_extent_cache (Axis);

  /* refpoints */
  Grob *common_refpoint (Grob const *s, Axis a) const;
  void set_parent (Grob *e, Axis);
  Grob *get_parent (Axis a) const;
  void fixup_refpoint ();
};

DECLARE_UNSMOB (Grob, grob);
Spanner *unsmob_spanner (SCM);
Item *unsmob_item (SCM);

/* refpoints */
Grob *common_refpoint_of_list (SCM elt_list, Grob *, Axis a);
Grob *common_refpoint_of_array (Link_array<Grob> const &, Grob *, Axis a);
System *get_root_system (Grob *me);

/* extents */ 
Interval robust_relative_extent (Grob *, Grob *, Axis);

/* offset/extent callbacks. */
void add_offset_callback (Grob *g, SCM proc, Axis a);
void chain_offset_callback (Grob *g, SCM proc, Axis a);
SCM axis_offset_symbol (Axis a);
SCM axis_parent_positioning (Axis a);


#endif /* GROB_HH */
