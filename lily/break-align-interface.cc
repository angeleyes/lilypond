/*
  break-align-interface.cc -- implement Break_align_interface

  source file of the GNU LilyPond music typesetter

  (c) 1997--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/


#include <math.h>
#include <libc-extension.hh>	// isinf

#include "self-alignment-interface.hh"
#include "side-position-interface.hh"
#include "axis-group-interface.hh"
#include "warn.hh"
#include "lily-guile.hh"
#include "break-align-interface.hh"
#include "dimensions.hh"
#include "output-def.hh"
#include "paper-column.hh"
#include "group-interface.hh"
#include "align-interface.hh"

MAKE_SCHEME_CALLBACK (Break_align_interface,alignment_callback,2);

SCM
Break_align_interface::alignment_callback (SCM element_smob, SCM axis)
{
  Grob *me = unsmob_grob (element_smob);
  Axis a = (Axis) scm_to_int (axis);

  assert (a == X_AXIS);
  Grob *par = me->get_parent (a);
  if (par && !to_boolean (par->get_property ("positioning-done")))
    {
      par->set_property ("positioning-done", SCM_BOOL_T);
      Break_align_interface::do_alignment (par);
    }
    
  return scm_make_real (0);
}

MAKE_SCHEME_CALLBACK (Break_align_interface,self_align_callback,2);
SCM
Break_align_interface::self_align_callback (SCM element_smob, SCM axis)
{
  Grob *me = unsmob_grob (element_smob);
  Axis a = (Axis) scm_to_int (axis);
  assert (a == X_AXIS);
  
  Item* item = dynamic_cast<Item*> (me);
  Direction bsd = item->break_status_dir ();
  if (bsd == LEFT)
    {
      me->set_property ("self-alignment-X", scm_int2num (RIGHT));
    }

  /*
    Force break alignment itself to be done first, in the case
   */
  return Self_alignment_interface::aligned_on_self (element_smob, axis);  
}


/*
  This is tricky: we cannot modify 'elements, since callers are
  iterating the same list. Reordering the list in-place, or resetting
  'elements will skip elements in the loops of callers.
  
  So we return the correct order as an array.
 */
Link_array<Grob>
Break_align_interface::ordered_elements (Grob *grob)
{
  Item *me  = dynamic_cast<Item*> (grob);
  SCM elts = me->get_property ("elements");
  SCM order_vec = me->get_property ("break-align-orders");
  if (!ly_c_vector_p (order_vec)
      || ly_vector_length (order_vec) < 3)
    return  Pointer_group_interface__extract_grobs (me, (Grob*)0,
						    "elements");
  SCM order = scm_vector_ref (order_vec,
			      scm_int2num (me->break_status_dir() + 1));


  /*
    Copy in order specified in BREAK-ALIGN-ORDER.
  */
  Link_array<Grob> new_elts;
  for (; scm_is_pair (order); order = scm_cdr (order))
    {
      SCM sym = scm_car (order);
      
      for (SCM s =elts; scm_is_pair (s); s = scm_cdr (s))
	{
	  Grob *g = unsmob_grob (scm_car (s));
	  if (g && sym == g->get_property ("break-align-symbol"))
	    {
	      new_elts.push (g);
	      elts = scm_delq (g->self_scm (), elts); 
	    }
	}
    }
  return new_elts;
}

void
Break_align_interface::add_element (Grob*me, Grob *toadd)
{
  Axis_group_interface::add_element (me, toadd);
}

void
Break_align_interface::do_alignment (Grob *grob)
{
  Item * me = dynamic_cast<Item*> (grob);

  
  Link_array<Grob> elems = ordered_elements (me);
  Array<Interval> extents;

  int last_nonempty = -1; 
  for (int i=0; i < elems.size (); i++) 
    {
      Interval y = elems[i]->extent (elems[i], X_AXIS);
      extents.push (y);
      if (!y.is_empty ())
	last_nonempty = i; 
    }

  int idx  = 0;
  while (idx < extents.size  () && extents[idx].is_empty ())
    idx++;
  
  Array<Real> offsets;
  offsets.set_size (elems.size ());
  for (int i= 0; i < offsets.size ();i ++)
    offsets[i] = 0.0;


  Real extra_right_space = 0.0;
  int edge_idx = -1;
  while (idx < elems.size ())
    {
      int next_idx = idx+1;
      while (next_idx < elems.size () &&
	     extents[next_idx].is_empty () )
	next_idx++;
      
      Grob *l = elems[idx];
      Grob *r = 0;

      if (next_idx < elems.size ())
	r = elems[next_idx];

      SCM alist = SCM_EOL;


      /*
	Find the first grob with a space-alist entry.
       */
      for (SCM s = l->get_property ("elements");
	   scm_is_pair (s) ; s = scm_cdr (s))
	  {
	    Grob *elt = unsmob_grob (scm_car (s));

	    if (edge_idx < 0
		&& elt->get_property ("break-align-symbol")
		== ly_symbol2scm ( "left-edge"))
	      edge_idx = idx;
	    
	    SCM l =elt->get_property ("space-alist");
	    if (scm_is_pair (l))
	      {
		alist= l;
		break;
	      }
	  }

      SCM rsym = r ? SCM_EOL : ly_symbol2scm ("right-edge");

      /*
	We used to use #'cause to find out the symbol and the spacing
	table, but that gets icky when that grob is suicided for some
	reason.
      */
      for (SCM s = r ? r->get_property ("elements") : SCM_EOL;
	   !scm_is_symbol (rsym) && scm_is_pair (s); s = scm_cdr (s))
	{
	  Grob * elt =unsmob_grob (scm_car (s));

	  rsym = elt->get_property ("break-align-symbol");
	}
	
      if (rsym  == ly_symbol2scm ("left-edge"))
	edge_idx = next_idx;

      SCM entry = SCM_EOL;
      if (scm_is_symbol (rsym))
	entry = scm_assq (rsym, alist);

      bool entry_found = scm_is_pair (entry);
      if (!entry_found)
	{
	  String sym_string;
	  if (scm_is_symbol (rsym))
	    sym_string = ly_symbol2string (rsym);

	  String orig_string ;
	  if (unsmob_grob (l->get_property ("cause")))
	    orig_string = unsmob_grob (l->get_property ("cause"))->name ();
	  
	  programming_error (_f ("No spacing entry from %s to `%s'",
				orig_string.to_str0 (),
				sym_string.to_str0 ()));
	}

      Real distance = 1.0;
      SCM type = ly_symbol2scm ("extra-space");
      
      if (entry_found)
	{
	  entry = scm_cdr (entry);
	  
	  distance = scm_to_double (scm_cdr (entry));
	  type = scm_car (entry) ;
	}

      if (r)
	{
	  if (type == ly_symbol2scm ("extra-space"))
	    offsets[next_idx] = extents[idx][RIGHT] + distance
	      - extents[next_idx][LEFT];
	  /* should probably junk minimum-space */
	  else if (type == ly_symbol2scm ("minimum-space"))
	    offsets[next_idx] = extents[idx][RIGHT] >? distance;
	}
      else
	{
	  extra_right_space = distance;	  
	}
      
      idx = next_idx;
    }

  Real here = 0.0;
  Interval total_extent;

  Real alignment_off =0.0;  
  for (int i =0 ; i < offsets.size (); i++)
    {
      here += offsets[i];
      if (i == edge_idx)
	alignment_off = -here; 
      total_extent.unite (extents[i] + here);
    }


  if (me->break_status_dir () == LEFT)
    {
      alignment_off = - total_extent[RIGHT] - extra_right_space;
    }
  else if (edge_idx < 0)
    alignment_off = -total_extent[LEFT];

  here = alignment_off;
  for (int i =0 ; i < offsets.size (); i++)
    {
      here += offsets[i];
      elems[i]->translate_axis (here, X_AXIS);
    }
}


ADD_INTERFACE (Break_aligned_interface, "break-aligned-interface",
	       "Items that are aligned in prefatory matter.\n"
	       "\n"
	       "The spacing of these items is controlled by the @code{space-alist}\n"
	       "property. It contains a list @code{break-align-symbol}s with a specification\n"
	       "of the associated space. The space specification can be "
	       "@table @code\n"
	       "@item (minimum-space . @var{spc}))\n"
	       "  Pad space until the distance is @var{spc}\n"
	       "@item (fixed-space . @var{spc})\n"
	       "  Set a fixed space\n" 
	       "@item (semi-fixed-space . @var{spc})\n"
	       "  Set a space. Half of it is fixed and half is stretchable. \n"
	       "(does not work at start of line. fixme)\n"
	       "@item (extra-space . @var{spc})\n"
	       "  Add @var{spc} amount of space.\n"
	       "@end table\n"
	       "\n"
	       "Special keys for the alist are @code{first-note} and @code{next-note}, signifying\n"
	       "the first note on a line, and the next note halfway a line.\n"
	       "\n"
	       "Rules for this spacing are much more complicated than this. \n"
	       "See [Wanske] page 126 -- 134, [Ross] pg 143 -- 147\n",
	       "break-align-symbol space-alist");

ADD_INTERFACE (Break_align_interface, "break-alignment-interface",
	       "The object that performs break aligment. See @ref{break-aligned-interface}.",
	       "positioning-done break-align-orders");



