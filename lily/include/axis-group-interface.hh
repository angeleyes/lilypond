/*
  axis-group-interface.hh -- declare Axis_group_interface

  source file of the GNU LilyPond music typesetter

  (c) 2000--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#ifndef AXIS_GROUP_INTERFACE_HH
#define AXIS_GROUP_INTERFACE_HH

#include "std-vector.hh"
#include "lily-proto.hh"
#include "grob-interface.hh"
#include "skyline.hh"

struct Axis_group_interface
{
  static SCM generic_group_extent (Grob *me, Axis a);
  static Interval pure_group_height (Grob *me, int start, int end);
  DECLARE_SCHEME_CALLBACK (width, (SCM smob));
  DECLARE_SCHEME_CALLBACK (calc_x_common, (SCM smob));
  DECLARE_SCHEME_CALLBACK (calc_y_common, (SCM smob));
  DECLARE_SCHEME_CALLBACK (height, (SCM smob));
  DECLARE_SCHEME_CALLBACK (pure_height, (SCM smob, SCM start, SCM end));
  DECLARE_SCHEME_CALLBACK (calc_skylines, (SCM smob));
  DECLARE_SCHEME_CALLBACK (combine_skylines, (SCM smob));
  DECLARE_SCHEME_CALLBACK (calc_max_stretch, (SCM smob));
  DECLARE_SCHEME_CALLBACK (print, (SCM smob));
  DECLARE_SCHEME_CALLBACK (adjacent_pure_heights, (SCM));
  static Interval relative_group_extent (vector<Grob*> const &list,
					 Grob *common, Axis);
  static Interval relative_pure_height (Grob *me, int start, int end);
  static Interval cached_pure_height (Grob *me, int, int);

  static Grob *calc_pure_elts_and_common (Grob*);
  static Skyline_pair skyline_spacing (Grob *me, vector<Grob*> elements);
  static void add_element (Grob *me, Grob *);
  static void set_axes (Grob *, Axis, Axis);
  static bool has_axis (Grob *, Axis);
  static void get_children (Grob *, vector<Grob*> *);
  static Interval staff_extent (Grob *me, Grob *ref, Axis, Grob *staff, Axis);
  static SCM calc_common (Grob *, Axis);
  DECLARE_GROB_INTERFACE();
};

#endif /* AXIS_GROUP_INTERFACE_HH */

