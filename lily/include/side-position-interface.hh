/*
  side-position-interface.hh -- declare Side_position_interface

  source file of the GNU LilyPond music typesetter

  (c) 1999--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#ifndef SIDE_POSITION_INTERFACE_HH
#define SIDE_POSITION_INTERFACE_HH

#include "spanner.hh"
#include "item.hh"

/*
  TODO: move out unrelated callbacks.

  TODO: reduce number of methods.
*/
struct Side_position_interface
{
public:

  DECLARE_SCHEME_CALLBACK (y_aligned_on_support_refpoints, (SCM element));
  DECLARE_SCHEME_CALLBACK (x_aligned_side, (SCM element));
  DECLARE_SCHEME_CALLBACK (y_aligned_side, (SCM element));

  static SCM Side_position_interface::aligned_side (Grob*me, Axis a);

  static SCM general_side_position (Grob *, Axis, bool);
  static Axis get_axis (Grob *);
  static void set_axis (Grob *, Axis);
  static bool has_interface (Grob *);
  static void add_support (Grob *, Grob *);
  static void add_staff_support (Grob *);
  static Direction get_direction (Grob *);
};

#endif /* SIDE_POSITION_INTERFACE_HH */

