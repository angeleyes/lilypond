/*
  line-spanner.cc -- implement Line_spanner

  source file of the GNU LilyPond music typesetter

  (c) 2000--2003 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include "molecule.hh"
#include "item.hh"
#include "spanner.hh"
#include "line-spanner.hh"
#include "paper-def.hh"
#include "paper-column.hh"
#include "staff-symbol-referencer.hh"
#include "font-interface.hh"
#include "warn.hh"
#include "align-interface.hh"

#include <math.h>


/*
  slightishly clumsy interface?

  Make  a Scheme expression for a line going from (0,0) to (dx,dy). 
 */

static SCM
line_atom (Grob *me, Real thick, Real dx, Real dy)
{
  SCM type = me->get_grob_property ("style");
  Real staff_space = Staff_symbol_referencer::staff_space (me);
  
      // maybe these should be in line-thickness?
  Real length = staff_space;
  SCM s = me->get_grob_property ("dash-length");
  if (gh_number_p (s))
    length = gh_scm2double (s) * staff_space;

  Real period = 2 * length + thick;
  s = me->get_grob_property ("dash-period");
  if (gh_number_p (s))
    period = gh_scm2double (s) * staff_space;
      
  if (type == ly_symbol2scm ("dotted-line"))
    length = thick;
	
  if (type == ly_symbol2scm ("line"))
    length = period + thick;

  Real on = length - thick;
  Real off = period - on;

  SCM list = scm_list_n (ly_symbol2scm ("dashed-line"),
		      gh_double2scm (thick),
		      gh_double2scm (on),
		      gh_double2scm (off),
		      gh_double2scm (dx),
		      gh_double2scm (dy),
		      SCM_UNDEFINED);

  return list;
}

static SCM
zigzag_atom (Grob *me, Real thick, Real dx, Real dy)
{
  Real staff_space = Staff_symbol_referencer::staff_space (me);
  SCM ws = me->get_grob_property ("zigzag-width");
  SCM ls = me->get_grob_property ("zigzag-length");
  double w = (gh_number_p(ws) ? gh_scm2double(ws) : 1)*staff_space;
  double l = (gh_number_p(ls) ? gh_scm2double(ls) : 1)*w;
  double h = l>w/2 ? sqrt(l*l-w*w/4) : 0;
  
  SCM list = scm_list_n (ly_symbol2scm ("zigzag-line"),
		      gh_bool2scm (true),
		      gh_double2scm (w),
		      gh_double2scm (h),
		      gh_double2scm (thick),
		      gh_double2scm (dx),
		      gh_double2scm (dy),
		      SCM_UNDEFINED);

  return list;
}

MAKE_SCHEME_CALLBACK(Line_spanner, after_line_breaking, 1);
SCM
Line_spanner::after_line_breaking (SCM  g)
{
  Grob *me  = unsmob_grob (g);
  Spanner*sp = dynamic_cast<Spanner*> (me);

  /*
    We remove the line at the start of the line.  For piano voice
    indicators, it makes no sense to have them at the start of the
    line.

    I'm not sure what the official rules for glissandi are, but
    usually the 2nd note of the glissando is "exact", so when playing
    from the start of the line, there is no need to glide.

    From a typographical p.o.v. this makes sense, since the amount of
    space left of a note at the start of a line is very small.

    --hwn.
    
   */
  if (sp->get_bound (LEFT)->break_status_dir()
      && !sp->get_bound (RIGHT)->break_status_dir())
    {
      /*
	Can't do suicide, since this mucks up finding the trend.
       */
      me->set_grob_property ("molecule-callback", SCM_EOL);
      
    }
  return SCM_EOL;
}


Molecule
Line_spanner::line_molecule (Grob *me, Real thick, Real dx, Real dy)
{
  Molecule mol;
  SCM type = me->get_grob_property ("style");
  if (gh_symbol_p (type)
      && (type == ly_symbol2scm ("line")
	  || type == ly_symbol2scm ("dashed-line")
	  || type == ly_symbol2scm ("dotted-line")
	  || (type == ly_symbol2scm ("trill") && dy != 0)))
    {
      Box b (Interval (-0.5* thick +  (0 <? dx) ,0.5* thick+ (0 >? dx)),
	     Interval (- 0.5* thick + (0<? dy), 0.5*thick + (0 >? dy)));
      mol = Molecule (b, line_atom (me, thick, dx, dy));
    }
  else if (gh_symbol_p (type)
	   && type == ly_symbol2scm ("zigzag"))
    {
      // TODO:
      Box b (Interval (-0.5* thick +  (0 <? dx) ,0.5* thick+ (0 >? dx)),
	     Interval (- 0.5* thick + (0<? dy), 0.5*thick + (0 >? dy)));
      mol = Molecule (b, zigzag_atom (me, thick, dx, dy));

    }
  else if (gh_symbol_p (type)
	   && type == ly_symbol2scm ("trill"))
    {
      SCM alist_chain = Font_interface::font_alist_chain (me);
      SCM style_alist = scm_list_n (gh_cons (ly_symbol2scm ("font-family"),
					     ly_symbol2scm ("music")),
				    SCM_UNDEFINED);
      
      Font_metric *fm = Font_interface::get_font (me,
						  gh_cons (style_alist,
							   alist_chain));
      Molecule m = fm->find_by_name ("scripts-trill-element");
      do
	mol.add_at_edge (X_AXIS, RIGHT, m, 0,0);
      while (m.extent (X_AXIS).length ()
	     && mol.extent (X_AXIS).length ()
	     + m.extent (X_AXIS).length () < dx);

      /*
	FIXME: should center element on x/y
       */
      mol.translate_axis (m.extent (X_AXIS).length () / 2, X_AXIS);
      mol.translate_axis (-(mol.extent (Y_AXIS)[DOWN]
			    + mol.extent (Y_AXIS).length ())/2, Y_AXIS); 
    }
  return mol;
}

/*
  Find a common Y parent, which --if found-- should be the
  fixed-distance alignment.
 */
Grob *
line_spanner_common_parent (Grob *me)
{
  Grob * common = find_fixed_alignment_parent (me);
  if (!common)
    {
      common = Staff_symbol_referencer::get_staff_symbol (me);
      if (common)
	common = common->get_parent (Y_AXIS);
      else
	common = me->get_parent (Y_AXIS);
    }

  return common;
}

/*
  Warning: this thing is a cross-staff object, so it should have empty Y-dimensions.

 (If not, you risk that this is called from the staff-alignment
  routine, via molecule_extent. At this point, the staves aren't
  separated yet, so it doesn't work cross-staff.

*/


MAKE_SCHEME_CALLBACK (Line_spanner, brew_molecule, 1);
SCM
Line_spanner::brew_molecule (SCM smob) 
{
  Spanner *me = dynamic_cast<Spanner*> (unsmob_grob (smob));

  Drul_array<Item*>  bound (me->get_bound (LEFT),
			    me->get_bound (RIGHT));
  
  
  Real gap = gh_scm2double (me->get_grob_property ("gap"));

  Offset ofxy (gap, 0); /*offset from start point to start of line*/
  Offset dxy ;
  Offset my_off;
  Offset his_off;
  
  Real thick = me->get_paper ()->get_var ("linethickness");  

  SCM s = me->get_grob_property ("thickness");
  if (gh_number_p (s))
    thick *= gh_scm2double (s);

  if (bound[RIGHT]->break_status_dir())
    {
      if (bound[LEFT]->break_status_dir ())
	{
	  programming_error ("line-spanner with two broken ends. Farewell sweet world.");

	  me->suicide();
	  return SCM_EOL;
	}

      /*
	This is hairy. For the normal case, we simply find common
	parents, and draw a line between the bounds. When two note
	heads are on different lines, there is no common parent
	anymore. We have to find the piano-staff object.
      */
      
      int k = broken_spanner_index (me);
      Spanner *parent_sp = dynamic_cast<Spanner*> (me->original_);
      Spanner *next_sp  = parent_sp->broken_intos_ [k+1];
      Item *next_bound = next_sp->get_bound (RIGHT);

      if (next_bound->break_status_dir ())
	{
	  programming_error ("no note heads for the line spanner on next line?"
			     " Confused.");
	  me->suicide();
	  return SCM_EOL;
	}
            
      Grob *commonx = bound[LEFT]->common_refpoint (bound[RIGHT], X_AXIS);
      commonx = me->common_refpoint (commonx, X_AXIS);
      
      Grob *next_common_y = line_spanner_common_parent (next_bound);
      Grob *this_common_y = line_spanner_common_parent (bound[LEFT]);

      Grob *all_common_y = me->common_refpoint (this_common_y, Y_AXIS);
      
      Interval next_ext  = next_bound->extent (next_common_y, Y_AXIS);
      Interval this_ext  = bound[LEFT]->extent (this_common_y, Y_AXIS);

      Real yoff = this_common_y->relative_coordinate (all_common_y, Y_AXIS);
      
      Offset p1 (bound[LEFT]->extent (commonx, X_AXIS)[RIGHT],
		 this_ext.center ()  + yoff); 
      Offset p2 (bound[RIGHT]->extent (commonx, X_AXIS)[LEFT],
		 next_ext.center () + yoff);
      
      Offset dz (p2 -p1);
      Real len = dz.length ();

      Offset dir  = dz *(1/ len);
      dz = (dz.length () - 2*gap) *dir;
      
  
      Molecule l (line_molecule (me, thick, dz[X_AXIS],
				 dz[Y_AXIS]));

      l.translate (dir * gap +  p1
		   - Offset (me->relative_coordinate (commonx, X_AXIS),
			     me->relative_coordinate (all_common_y, Y_AXIS)));

      return l.smobbed_copy (); 
    }
  else
    {
      Grob *common[] = { me, me };
      for (int a = X_AXIS;  a < NO_AXES; a++)
	{
	  common[a] = me->common_refpoint (bound[RIGHT], Axis (a));
	  common[a] = common[a]->common_refpoint (bound[LEFT], Axis (a));
	}

      // distance from center to start of line      
      Real off = gap + ((bound[LEFT]->extent (bound[LEFT], X_AXIS).length ()*3)/4);

      for (int a = X_AXIS; a < NO_AXES; a++)
	{
	  Axis ax = (Axis)a;
	  dxy[ax] =
	    + bound[RIGHT]->extent (common[X_AXIS], ax).center ()
	    - bound[LEFT]->extent (common[X_AXIS], ax).center ();

	  my_off[ax] =me->relative_coordinate (common[a], ax);
	  his_off[ax] = bound[LEFT]->relative_coordinate (common[a], ax);
	  
	}

      ofxy = dxy * (off/dxy.length ());
      dxy -= 2*ofxy;
  
      Molecule line = line_molecule (me, thick, dxy[X_AXIS], dxy[Y_AXIS]);
      line.translate_axis (bound[LEFT]->extent (bound[LEFT],
						X_AXIS).length ()/2, X_AXIS); 
      line.translate (ofxy - my_off + his_off);
      return line.smobbed_copy ();
    }
}


ADD_INTERFACE (Line_spanner, "line-spanner-interface",
  "Generic line drawn between two objects, eg. for use with glissandi.\n"
"gap is measured in staff-spaces.\n"
"The property 'type is one of: line, dashed-line, trill, dotted-line or zigzag.\n"
"\n",
  "gap dash-period dash-length zigzag-width zigzag-length thickness style");


