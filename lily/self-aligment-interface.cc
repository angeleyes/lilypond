#include "self-alignment-interface.hh"
#include "warn.hh"

/*
  Position centered on parent.
 */
MAKE_SCHEME_CALLBACK (Self_alignment_interface,centered_on_parent,2);
SCM
Self_alignment_interface::centered_on_parent (SCM element_smob, SCM axis)
{
  Grob *me = unsmob_grob (element_smob);
  Axis a = (Axis) gh_scm2int (axis);
  Grob *him = me->get_parent (a);
  Interval he = him->extent (him,a);
  
  return  gh_double2scm (he.is_empty () ? 0.0 : he.center ());
}

MAKE_SCHEME_CALLBACK (Self_alignment_interface,aligned_on_parent,2);
SCM
Self_alignment_interface::aligned_on_parent (SCM element_smob, SCM axis)
{
  Grob *me = unsmob_grob (element_smob);
  Axis a = (Axis) gh_scm2int (axis);
  Grob *him = me->get_parent (a);
  Interval he = him->extent (him,a);
  
  SCM sym= (a == X_AXIS) ? ly_symbol2scm ("self-alignment-X"): ly_symbol2scm ("self-alignment-Y");
  SCM align_prop (me->internal_get_grob_property (sym));

  if (!gh_number_p (align_prop))
    return gh_int2scm (0);

  Real x = 0.0;

  Real align = gh_scm2double (align_prop);
      
  Interval ext (me->extent (me,a));
  if (ext.is_empty ())
    {
      programming_error ("I'm empty. Can't align on self");
      
    }
  else
    x -= ext.linear_combination (align) ;

  if (!he.is_empty ())
    {
      x += he.linear_combination (align);
    }

  return gh_double2scm (x);
}

/*
  Position centered on parent.
 */
MAKE_SCHEME_CALLBACK (Self_alignment_interface,centered_on_other_axis_parent,2);
SCM
Self_alignment_interface::centered_on_other_axis_parent (SCM element_smob,
							 SCM axis)
{
  Grob *me = unsmob_grob (element_smob);
  Axis a = (Axis) gh_scm2int (axis);
  Grob *him = me->get_parent (other_axis (a));
  Interval he = him->extent (him,a);
  
  return  gh_double2scm (he.is_empty () ? 0.0 : he.center ());
}




/**
  callback that centers the element on itself

  Requires that self-alignment-{X,Y} be set.
 */
MAKE_SCHEME_CALLBACK (Self_alignment_interface,aligned_on_self,2);
SCM
Self_alignment_interface::aligned_on_self (SCM element_smob, SCM axis)
{
  Grob *me = unsmob_grob (element_smob);
  Axis a = (Axis) gh_scm2int (axis);

  SCM sym= (a == X_AXIS) ? ly_symbol2scm ("self-alignment-X"): ly_symbol2scm ("self-alignment-Y");
  
  SCM align (me->internal_get_grob_property (sym));
  if (gh_number_p (align))
    {
      Interval ext (me->extent (me,a));

      if (ext.is_empty ())
	{
	  programming_error ("I'm empty. Can't align on self");
	  return gh_double2scm (0.0);
	}
      else
	{
	  return gh_double2scm (- ext.linear_combination (gh_scm2double (align)));
	}
    }
  return gh_double2scm (0.0);
}


ADD_INTERFACE (Self_alignment_interface, "self-alignment-interface",
	       "Position this object on itself and/or on its parent. To this end, the following functions "
	       " are provided: \n"
	       "@table @code \n"
	       "@item Self_alignment_interface::aligned_on_self\n"
	       "  Align self on reference point, using @code{self-alignment-X} and "
	       "@code{self-alignment-Y}."
	       "@item Self_alignment_interface::aligned_on_parent\n"
	       "@item Self_alignment_interface::centered_on_parent\n"
	       "  Shift the object so its own reference point is centered on the  "
	       " extent of the parent \n"
	       "@item Self_alignment_interface::centered_on_other_axis_parent\n"
	       " For X-axis, center on the Y-parent, and vice versa.\n "
	       "@end table\n"
	       ,
	       "self-alignment-X self-alignment-Y");

