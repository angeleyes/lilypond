/*
  self-alignment-interface.cc -- implement Self_alignment_interface
 
  source file of the GNU LilyPond music typesetter

  (c) 2004--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "self-alignment-interface.hh"
#include "warn.hh"

MAKE_SCHEME_CALLBACK (Self_alignment_interface, y_aligned_on_self, 1);
SCM
Self_alignment_interface::y_aligned_on_self (SCM element)
{
  return aligned_on_self (unsmob_grob (element), Y_AXIS);
}

MAKE_SCHEME_CALLBACK (Self_alignment_interface, x_aligned_on_self, 1);
SCM
Self_alignment_interface::x_aligned_on_self (SCM element)
{
  return aligned_on_self (unsmob_grob (element), X_AXIS);
}

SCM
Self_alignment_interface::aligned_on_self (Grob *me, Axis a)
{
  SCM sym = (a == X_AXIS) ? ly_symbol2scm ("self-alignment-X")
    : ly_symbol2scm ("self-alignment-Y");

  SCM align (me->internal_get_property (sym));
  if (scm_is_number (align))
    {
      Interval ext (me->extent (me, a));
      if (ext.is_empty ())
	programming_error ("can't align on self: empty element");
      else
	return scm_from_double (- ext.linear_combination (scm_to_double (align)));
    }
  return scm_from_double (0.0);
}



SCM
Self_alignment_interface::centered_on_object (Grob *him, Axis a)
{
  return scm_from_double (robust_relative_extent (him, him, a).center ());
}


MAKE_SCHEME_CALLBACK (Self_alignment_interface, centered_on_x_parent, 1);
SCM
Self_alignment_interface::centered_on_x_parent (SCM smob)
{
  return centered_on_object (unsmob_grob (smob)->get_parent (X_AXIS), X_AXIS);
}


MAKE_SCHEME_CALLBACK (Self_alignment_interface, centered_on_y_parent, 1);
SCM
Self_alignment_interface::centered_on_y_parent (SCM smob)
{
  return centered_on_object (unsmob_grob (smob)->get_parent (Y_AXIS), Y_AXIS);
}


MAKE_SCHEME_CALLBACK (Self_alignment_interface, x_centered_on_y_parent, 1);
SCM
Self_alignment_interface::x_centered_on_y_parent (SCM smob)
{
  return centered_on_object (unsmob_grob (smob)->get_parent (Y_AXIS), X_AXIS);
}

MAKE_SCHEME_CALLBACK (Self_alignment_interface, aligned_on_x_parent,1);
SCM
Self_alignment_interface::aligned_on_x_parent (SCM smob)
{
  return aligned_on_parent (unsmob_grob (smob), X_AXIS);
}

MAKE_SCHEME_CALLBACK (Self_alignment_interface, aligned_on_y_parent,1);
SCM
Self_alignment_interface::aligned_on_y_parent (SCM smob)
{
  return aligned_on_parent (unsmob_grob (smob), Y_AXIS);
}

SCM
Self_alignment_interface::aligned_on_parent (Grob *me, Axis a)
{
  Grob *him = me->get_parent (a);
  Interval he = him->extent (him, a);

  SCM sym = (a == X_AXIS) ? ly_symbol2scm ("self-alignment-X")
    : ly_symbol2scm ("self-alignment-Y");
  SCM align_prop (me->internal_get_property (sym));

  if (!scm_is_number (align_prop))
    return scm_from_int (0);

  Real x = 0.0;
  Real align = scm_to_double (align_prop);

  Interval ext (me->extent (me, a));
  if (ext.is_empty ())
    programming_error ("can't align on self: empty element");
  else
    x -= ext.linear_combination (align);

  if (!he.is_empty ())
    x += he.linear_combination (align);

  return scm_from_double (x);
}

void
Self_alignment_interface::set_center_parent (Grob *me, Axis a)
{
  add_offset_callback (me,
		       (a==X_AXIS) ? centered_on_x_parent_proc : centered_on_y_parent_proc,
		       a);
}

void
Self_alignment_interface::set_align_self (Grob *me, Axis a)
{
  add_offset_callback (me,
		       (a==X_AXIS) ? x_aligned_on_self_proc : y_aligned_on_self_proc,
		       a);
}

ADD_INTERFACE (Self_alignment_interface, "self-alignment-interface",
	       "Position this object on itself and/or on its parent. To this end, the following functions "
	       " are provided: \n"
	       "@table @code \n"
	       "@item Self_alignment_interface::[xy]_aligned_on_self\n"
	       "  Align self on reference point, using @code{self-alignment-X} and "
	       "@code{self-alignment-Y}."
	       "@item Self_alignment_interface::aligned_on_[xy]_parent\n"
	       "@item Self_alignment_interface::centered_on_[xy]_parent\n"
	       "  Shift the object so its own reference point is centered on the  "
	       " extent of the parent \n"
	       "@end table\n",


	       /* properties */
	       "self-alignment-X "
	       "self-alignment-Y ");

