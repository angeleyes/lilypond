/*   
  multi-measure-rest.cc --  implement Multi_measure_rest
  
  source file of the GNU LilyPond music typesetter
  
  (c) 1998--2004 Jan Nieuwenhuizen <janneke@gnu.org>
  
 */

#include "multi-measure-rest.hh"
#include "warn.hh"
#include "paper-def.hh"
#include "paper-column.hh" // urg
#include "font-interface.hh"
#include "rest.hh"
#include "stencil.hh"
#include "misc.hh"
#include "spanner.hh"
#include "staff-symbol-referencer.hh"
#include "text-item.hh"
#include "percent-repeat-item.hh"
#include "lookup.hh"
#include "separation-item.hh"

MAKE_SCHEME_CALLBACK (Multi_measure_rest,percent,1);
SCM
Multi_measure_rest::percent (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  Spanner *sp = dynamic_cast<Spanner*> (me);
  
  Stencil r = Percent_repeat_item_interface::x_percent (me, 1,  0.75, 1.6);

  // ugh copy & paste.
  
  Interval sp_iv;
  Direction d = LEFT;
  do
    {
      Item * col = sp->get_bound (d)->get_column ();

      Interval coldim = col->extent (0, X_AXIS);

      sp_iv[d] = coldim[-d]  ;
    }
  while ((flip (&d)) != LEFT);
  Real x_off = 0.0;

  Real rx  = sp->get_bound (LEFT)->relative_coordinate (0, X_AXIS);
  /*
    we gotta stay clear of sp_iv, so move a bit to the right if
    needed.
   */
  x_off += (sp_iv[LEFT] -  rx) >? 0;

  /*
    center between stuff.
   */
  x_off += sp_iv.length ()/ 2;

  r.translate_axis (x_off,X_AXIS);

  
  return r.smobbed_copy ();
}

MAKE_SCHEME_CALLBACK (Multi_measure_rest,print,1);
SCM
Multi_measure_rest::print (SCM smob) 
{
  Grob *me = unsmob_grob (smob);
  Spanner * sp = dynamic_cast<Spanner*> (me);

  Interval sp_iv;
  Direction d = LEFT;

  Grob *common = sp->get_bound (LEFT)->common_refpoint (sp->get_bound (RIGHT), X_AXIS);
  do
    {
      Item * b = sp->get_bound (d);

      Interval coldim =  (Separation_item::has_interface (b))
	? Separation_item::relative_width (b, common)
	: b->extent (common, X_AXIS);

      sp_iv[d] = coldim.is_empty () ?   b->relative_coordinate (common, X_AXIS) : coldim[-d];
    }
  while ((flip (&d)) != LEFT);

  Real space = sp_iv.length();

  Real rx  = sp->get_bound (LEFT)->relative_coordinate (0, X_AXIS);
  /*
    we gotta stay clear of sp_iv, so move a bit to the right if
    needed.
   */
  Real x_off = (sp_iv[LEFT] -  rx) >? 0;


  Stencil mol;
  mol.add_stencil (symbol_stencil (me, space));

  int measures = 0;
  SCM m (me->get_grob_property ("measure-count"));
  if (gh_number_p (m))
    {
      measures = gh_scm2int (m);
    }

  mol.translate_axis (x_off, X_AXIS);
  return mol.smobbed_copy ();
}



Stencil
Multi_measure_rest::symbol_stencil (Grob *me, Real space)
{
  int measures = 0;
  SCM m (me->get_grob_property ("measure-count"));
  if (gh_number_p (m))
    {
      measures = gh_scm2int (m);
    }
  if (measures <= 0)
    return Stencil();
  

  SCM limit = me->get_grob_property ("expand-limit");
  if (measures > gh_scm2int (limit))
    {
      Real padding = 0.15;  
      Stencil s =  big_rest (me, (1.0 - 2*padding) * space);
      s.translate_axis (padding * space,  X_AXIS); 
      return s;
    }

  SCM alist_chain = Font_interface::font_alist_chain (me);

  Real staff_space = Staff_symbol_referencer::staff_space (me);
  Font_metric *musfont
    = select_font (me->get_paper (), alist_chain);

  SCM sml = me->get_grob_property ("use-breve-rest");
  if (measures == 1)
    {
      if (sml == SCM_BOOL_T)
	{
	  Stencil s = musfont->find_by_name (Rest::glyph_name (me, -1, ""));

	  s.translate_axis ((space - s.extent (X_AXIS).length ())/2, X_AXIS);
      
	  return s ;
	}
      else
	{
	  Stencil s = musfont->find_by_name (Rest::glyph_name (me, 0, ""));

	  /*
	    ugh.
	   */
	  if (Staff_symbol_referencer::get_position (me) == 0.0)
	    s.translate_axis (staff_space, Y_AXIS);

	  s.translate_axis ((space - s.extent (X_AXIS).length ())/2, X_AXIS);
      
	  return s ;
        }
    }
  else
    {
      return  church_rest (me, musfont, measures, space);
    }
}


/*
  WIDTH can also be 0 to determine the minimum size of the object.
 */
Stencil
Multi_measure_rest::big_rest (Grob *me, Real width)
{
  Real thick_thick =robust_scm2double (me->get_grob_property ("thick-thickness"), 1.0);
  Real hair_thick = robust_scm2double (me->get_grob_property ("hair-thickness"), .1);


  Real ss = Staff_symbol_referencer::staff_space (me);
  Real slt = me->get_paper ()->get_realvar (ly_symbol2scm ("linethickness"));
  Real y = slt * thick_thick/2 * ss;
  Real ythick = hair_thick * slt * ss;
  Box b(Interval (0,  0 >? (width - 2 * ythick)), Interval (-y, y));
  
  Real blot = width ? (.8 * (y <? ythick)) : 0.0;
  
  Stencil m =  Lookup::round_filled_box (b, blot);
  Stencil yb = Lookup::round_filled_box (Box (Interval (-0.5, 0.5)* ythick, Interval (-ss, ss)), blot);

  m.add_at_edge (X_AXIS, RIGHT, yb, 0, 0);
  m.add_at_edge (X_AXIS, LEFT, yb, 0, 0);

  m.align_to (X_AXIS, LEFT);
  
  return m;
}

/*
  Kirchenpause (?)
 */
Stencil
Multi_measure_rest::church_rest (Grob*me, Font_metric *musfont, int measures,
				 Real space)
{
  SCM mols = SCM_EOL; 

  /*
   see Wanske pp. 125
  */
  int l = measures;
  int count = 0;
  Real symbols_width = 0.0;

  SCM sml = me->get_grob_property ("use-breve-rest");

  while (l)
    {
      if (sml == SCM_BOOL_T)
	{
	  int k;
	  if (l >= 2)
	    {
	      l-=2;
	      k = -2;
	    }
	  else
	    {
	      l -= 1;
	      k = -1;
	    }

	  Stencil r (musfont->find_by_name ("rests-" + to_string (k)));
	  symbols_width += r.extent (X_AXIS).length ();
	  mols = gh_cons (r.smobbed_copy (), mols);
	}
       else
	{
	  int k;
	  if (l >= 4)
	    {
	      l-=4;
	      k = -2;
	    }
	  else if (l>= 2)
	    {
	      l -= 2;
	      k = -1;
	    }
	  else
	    {
	      k = 0;
	      l --;
	    }

	  Stencil r (musfont->find_by_name ("rests-" + to_string (k)));
	  if (k == 0)
	    {
	      Real staff_space = Staff_symbol_referencer::staff_space (me);
	      r.translate_axis (staff_space, Y_AXIS);
	    }
	  symbols_width += r.extent (X_AXIS).length ();
	  mols = gh_cons (r.smobbed_copy (), mols);
	}
      count ++;
    }

  

  Real outer_padding_factor = 1.5; //     make outer padding this much bigger.
  Real inner_padding = (space - symbols_width) / (2 * outer_padding_factor + (count-1)); 
  if (inner_padding < 0)
    {
      inner_padding = 1.0;
    }
  
  Stencil mol; 
  for (SCM  s = mols; gh_pair_p (s); s = gh_cdr(s))
    {
      mol.add_at_edge (X_AXIS, LEFT, *unsmob_stencil (gh_car (s)), inner_padding, 0);
    }
  mol.align_to (X_AXIS, LEFT);
  mol.translate_axis (outer_padding_factor *  inner_padding, X_AXIS);

  return mol;
}

void
Multi_measure_rest::add_column (Grob*me,Item* c)
{
  add_bound_item (dynamic_cast<Spanner*> (me),c);
}


MAKE_SCHEME_CALLBACK (Multi_measure_rest, set_spacing_rods,1);
SCM
Multi_measure_rest::set_spacing_rods (SCM smob)
{
  Grob*me = unsmob_grob (smob);

  Spanner*sp = dynamic_cast<Spanner*> (me);
  if (! (sp->get_bound (LEFT) && sp->get_bound (RIGHT)))
    {
      programming_error ("Multi_measure_rest::get_rods (): I am not spanned!");
      return SCM_UNSPECIFIED;
    }

  Item * l = sp->get_bound (LEFT)->get_column ();
  Item * r = sp->get_bound (RIGHT)->get_column ();
  Item * lb = l->find_prebroken_piece (RIGHT);
  Item * rb = r->find_prebroken_piece (LEFT);      
  
  Item* combinations[4][2]={{l,r}, {lb,r}, {l,rb},{lb,rb}};

  Real sym_width = symbol_stencil (me, 0.0).extent (X_AXIS).length ();
  
  for (int i=0; i < 4; i++)
    {
      Item * l =  combinations[i][0];
      Item *r = combinations[i][1];

      if (!l || !r)
	continue;

      Rod rod;
      rod.item_l_drul_[LEFT] = l;
      rod.item_l_drul_[RIGHT] = r;

      
      rod.distance_ = l->extent (l, X_AXIS)[BIGGER] - r->extent (r, X_AXIS)[SMALLER]
	+ sym_width  + 2.0;			// 2.0 = magic!
  
      Real minlen  = robust_scm2double (me->get_grob_property ("minimum-length"), 0.0);
      rod.distance_ = max(rod.distance_,
			  minlen);
      rod.add_to_cols ();
    }
  return SCM_UNSPECIFIED;
}




ADD_INTERFACE (Multi_measure_rest,"multi-measure-rest-interface",
	       "A rest that spans a whole number of measures.",
	       "expand-limit measure-count hair-thickness thick-thickness use-breve-rest minimum-length");

