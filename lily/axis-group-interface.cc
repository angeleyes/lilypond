/*   
  axis-group-interface.cc --  implement Axis_group_interface
  
  source file of the GNU LilyPond music typesetter
  
  (c) 2000--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  
 */
#include "hara-kiri-group-spanner.hh"
#include "axis-group-interface.hh"
#include "grob.hh"

void
Axis_group_interface::add_element (Grob*me,Grob *e)
{
  for (SCM ax = me->get_grob_property ("axes"); ax != SCM_EOL ; ax = ly_cdr (ax))
    {
      Axis a = (Axis) gh_scm2int (ly_car (ax));
      
      if (!e->get_parent (a))
	e->set_parent (me, a);
    }

  Pointer_group_interface::add_grob (me, ly_symbol2scm ("elements"), e);
  me->add_dependency (e);
}

bool
Axis_group_interface::axis_b (Grob*me,Axis a)
{
  /*
    urg. FIXME, check for Hara_kiri_group_spanner shouldn't be necessary?

    
   */
  return me->has_extent_callback_b (group_extent_callback_proc, a) ||
 (me->has_extent_callback_b (Hara_kiri_group_spanner::y_extent_proc, a));
}

Interval
Axis_group_interface::relative_group_extent (Axis a, Grob *common, SCM elts)
{
  Interval r;
  for (SCM s = elts; gh_pair_p (s); s = ly_cdr (s))
    {
      Grob * se = unsmob_grob (ly_car (s));
      Interval dims = se->extent (common, a);
      if (!dims.is_empty ())
	r.unite (dims);
    }
  return r;
}

MAKE_SCHEME_CALLBACK (Axis_group_interface,group_extent_callback,2);
SCM
Axis_group_interface::group_extent_callback (SCM element_smob, SCM scm_axis)
{
  Grob *me = unsmob_grob (element_smob);
  Axis a = (Axis) gh_scm2int (scm_axis);

  SCM elts = me->get_grob_property ("elements");
  Grob * common = common_refpoint_of_list (elts, me, a);

  Real my_coord = me->relative_coordinate (common, a);
  Interval r (relative_group_extent (a, common,elts));

  return ly_interval2scm (r - my_coord);
}

void
Axis_group_interface::set_axes (Grob*me,Axis a1, Axis a2)
{
  SCM sa1= scm_int2num (a1);
  SCM sa2 = scm_int2num (a2);

  SCM axes = me->get_grob_property ("axes");
  
  if (!gh_pair_p (axes)
      || scm_memq (sa1, axes) == SCM_BOOL_F
      || scm_memq (sa2, axes) == SCM_BOOL_F)
    {
      SCM ax = gh_cons (sa1, SCM_EOL);
      if (a1 != a2)
	ax= gh_cons (sa2, ax);
      me->set_grob_property ("axes", ax);
    }

  if (a1 != X_AXIS && a2 != X_AXIS)
    me->set_extent (SCM_EOL, X_AXIS);
  if (a1 != Y_AXIS && a2 != Y_AXIS)
    me->set_extent (SCM_EOL, Y_AXIS);

  /*
    why so convoluted ? (fixme/documentme?) 
   */
  if (me->has_extent_callback_b (Grob::molecule_extent_proc, a1))
    me->set_extent (Axis_group_interface::group_extent_callback_proc,a1);
  if (me->has_extent_callback_b (Grob::molecule_extent_proc, a2))
    me->set_extent (Axis_group_interface::group_extent_callback_proc,a2);
}

Link_array<Grob> 
Axis_group_interface::get_children (Grob*me)
{
  Link_array<Grob> childs;
  childs.push (me) ;

  if (!has_interface (me))
    return childs;
  
  for (SCM ep = me->get_grob_property ("elements"); gh_pair_p (ep); ep = ly_cdr (ep))
    {
      Grob* e = unsmob_grob (ly_car (ep));
      if (e)
	childs.concat (Axis_group_interface::get_children (e));
    }
  
  return childs;
}



ADD_INTERFACE (Axis_group_interface, "axis-group-interface",
  "a group of coupled grobs",
  "axes");
