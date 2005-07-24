/*
  system.cc -- implement System

  source file of the GNU LilyPond music typesetter

  (c) 1996--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "system.hh"

#include <math.h>

#include "align-interface.hh"
#include "axis-group-interface.hh"
#include "warn.hh"
#include "main.hh"
#include "paper-column.hh"
#include "output-def.hh"
#include "paper-score.hh"
#include "warn.hh"
#include "all-font-metrics.hh"
#include "spacing-interface.hh"
#include "staff-symbol-referencer.hh"
#include "paper-book.hh"
#include "paper-system.hh"
#include "tweak-registration.hh"
#include "grob-array.hh"
#include "pointer-group-interface.hh"

System::System (System const &src, int count)
  : Spanner (src, count)
{
  all_elements_ = 0;
  rank_ = 0;
  init_elements (); 
}

System::System (SCM s, Object_key const *key)
  : Spanner (s, key)
{
  all_elements_ = 0;
  rank_ = 0;
  init_elements (); 
}

void
System::init_elements ()
{
  SCM scm_arr = Grob_array::make_array ();
  all_elements_ = unsmob_grob_array (scm_arr);
  set_object ("all-elements", scm_arr);
}


Grob *
System::clone (int count) const
{
  return new System (*this, count);
}

int
System::element_count () const
{
  return all_elements_->size ();
}

int
System::spanner_count () const
{
  int k = 0;
  for (int i = all_elements_->size(); i--;)
    if (dynamic_cast<Spanner *> (all_elements_->grob (i)))
      k++;
  return k;
}

void
System::typeset_grob (Grob *elem)
{
  if (elem->pscore_)
    programming_error ("adding element twice");
  else
    {
      elem->pscore_ = pscore_;
      all_elements_->add (elem);
      elem->unprotect ();
    }
}

void
System::derived_mark () const
{
  if (!all_elements_->is_empty ())
    {
      Grob **ptr = &all_elements_->array_reference ().elem_ref (0);
      Grob **end = ptr + all_elements_->size ();
      while (ptr < end)
	{
	  scm_gc_mark ((*ptr)->self_scm ());
	  ptr ++;
	}
    }

  if (pscore_)
    scm_gc_mark (pscore_->self_scm ());
  
  Spanner::derived_mark ();
}

static void
fixup_refpoints (Link_array<Grob> const &grobs)
{
  for (int i = grobs.size (); i--; )
    {
      grobs[i]->fixup_refpoint ();
    }
}

SCM
System::get_paper_systems ()
{
  for (int i = 0; i < all_elements_->size(); i++)
    {
      Grob *g = all_elements_->grob (i);
      if (g->internal_has_interface (ly_symbol2scm ("only-prebreak-interface")))
	{
	  /*
	    Kill no longer needed grobs.
	  */
	  Item *it = dynamic_cast<Item *> (g);
	  if (it && Item::is_breakable (it))
	    {
	      it->find_prebroken_piece (LEFT)->suicide ();
	      it->find_prebroken_piece (RIGHT)->suicide ();
	    }
	  g->suicide ();
	}
      else if (g->is_live ())
	g->do_break_processing ();
    }

  /*
    fixups must be done in broken line_of_scores, because new elements
    are put over there.  */
  int count = 0;
  for (int i = 0; i < broken_intos_.size (); i++)
    {
      Grob *se = broken_intos_[i];
      
      extract_grob_set (se, "all-elements", all_elts);
      for (int j = 0; j < all_elts.size(); j++)
	{
	  Grob *g = all_elts[j];
	  g->fixup_refpoint ();
	}
      
      count += all_elts.size ();
    }

  /*
    needed for doing items.
  */
  fixup_refpoints (all_elements_->array ());
  
  for (int i = 0 ; i < all_elements_->size(); i++)
    all_elements_->grob (i)->handle_broken_dependencies ();

  handle_broken_dependencies ();
  
#if 0  /* don't do this: strange side effects.  */

  /* Because the this->get_property (all-elements) contains items in 3
     versions, handle_broken_dependencies () will leave duplicated
     items in all-elements.  Strictly speaking this is harmless, but
     it leads to duplicated symbols in the output.  ly_list_qsort_uniq_x ()
     makes sure that no duplicates are in the list.  */
  for (int i = 0; i < line_count; i++)
    {
      SCM all = broken_intos_[i]->get_object ("all-elements");
      all = ly_list_qsort_uniq_x (all);
    }
#endif

  if (be_verbose_global)
    message (_f ("Element count %d.", count + element_count ()));

  int line_count = broken_intos_.size ();
  SCM lines = scm_c_make_vector (line_count, SCM_EOL);

  for (int i = 0; i < line_count; i++)
    {
      if (be_verbose_global)
	progress_indication ("[");

      System *system = dynamic_cast<System *> (broken_intos_[i]);
      system->post_processing ();
      scm_vector_set_x (lines, scm_int2num (i), system->get_paper_system ());

      if (be_verbose_global)
	progress_indication (to_string (i) + "]");
    }
  return lines;
}

void
System::break_into_pieces (Array<Column_x_positions> const &breaking)
{
  for (int i = 0; i < breaking.size (); i++)
    {
      System *system = dynamic_cast<System *> (clone (i));
      system->rank_ = i;

      Link_array<Grob> c (breaking[i].cols_);
      pscore_->typeset_system (system);

      system->set_bound (LEFT, c[0]);
      system->set_bound (RIGHT, c.top ());
      for (int j = 0; j < c.size (); j++)
	{
	  c[j]->translate_axis (breaking[i].config_[j], X_AXIS);
	  dynamic_cast<Paper_column *> (c[j])->system_ = system;
	}
      set_loose_columns (system, &breaking[i]);
      broken_intos_.push (system);
    }
}

void
System::add_column (Paper_column *p)
{
  Grob *me = this;
  Grob_array *ga = unsmob_grob_array (me->get_object ("columns"));
  if (!ga)
    {
      SCM scm_ga = Grob_array::make_array ();
      me->set_object ("columns", scm_ga);
      ga = unsmob_grob_array (scm_ga);
    }

  p->rank_
    = ga->size()
    ? Paper_column::get_rank (ga->array ().top ()) + 1
    : 0;
    
  ga->add (p);
  Axis_group_interface::add_element (this, p);
}

void
apply_tweaks (Grob *g, bool broken)
{
  if (bool (g->original_) == broken)
    {
      SCM tweaks = global_registry_->get_tweaks (g);
      for (SCM s = tweaks; scm_is_pair (s); s = scm_cdr (s))
	{
	  SCM proc = scm_caar (s);
	  SCM rest = scm_cdar (s);
	  scm_apply_1 (proc, g->self_scm (), rest);
	}
    }
}

void
System::pre_processing ()
{
  for (int i = 0 ;  i < all_elements_->size(); i ++)
    all_elements_->grob (i)->discretionary_processing ();
  

  if (be_verbose_global)
    message (_f ("Grob count %d", element_count ()));

  /*
    order is significant: broken grobs are added to the end of the
    array, and should be processed before the original is potentially
    killed.
  */
  for (int i = all_elements_->size(); i --; )
    all_elements_->grob (i)->handle_prebroken_dependencies ();

  fixup_refpoints (all_elements_->array ());

  for (int i = 0 ;  i < all_elements_->size(); i ++)
    apply_tweaks (all_elements_->grob (i), false);

  for (int i = 0 ;  i < all_elements_->size(); i ++)
    all_elements_->grob (i)->calculate_dependencies (PRECALCED, PRECALCING,
						     ly_symbol2scm ("before-line-breaking-callback"));

  message (_ ("Calculating line breaks..."));
  progress_indication (" ");
  
  for (int i = 0 ;  i < all_elements_->size(); i ++)
    {
      Grob *e = all_elements_->grob (i);
      SCM proc = e->get_property ("spacing-procedure");
      if (ly_is_procedure (proc))
	scm_call_1 (proc, e->self_scm ());
    }
}

void
System::post_processing ()
{
  for (int i = 0 ;  i < all_elements_->size(); i ++)
    {
      Grob *g = all_elements_->grob (i);

      apply_tweaks (g, true);
      g->calculate_dependencies (POSTCALCED, POSTCALCING,
				 ly_symbol2scm ("after-line-breaking-callback"));
    }

  Interval iv (extent (this, Y_AXIS));
  if (iv.is_empty ())
    {
      programming_error ("system with empty extent");
    }
  else
    translate_axis (-iv[MAX], Y_AXIS);

  /* Generate all stencils to trigger font loads.
     This might seem inefficient, but Stencils are cached per grob
     anyway. */


  Link_array<Grob> all_elts_sorted (all_elements_->array ());
  all_elts_sorted.default_sort ();
  all_elts_sorted.uniq ();
  this->get_stencil ();
  for (int i = all_elts_sorted.size (); i--;)
    {
      Grob *g = all_elts_sorted[i];
      g->get_stencil ();
    }
}

SCM
System::get_paper_system ()
{
  static int const LAYER_COUNT = 3;

  SCM exprs = SCM_EOL;
  SCM *tail = &exprs;

  /* Output stencils in three layers: 0, 1, 2.  Default layer: 1. */
  for (int i = 0; i < LAYER_COUNT; i++)
    for (int j = all_elements_->size (); j --;)
      {
	Grob *g = all_elements_->grob (j);
	Stencil *stil = g->get_stencil ();

	/* Skip empty stencils and grobs that are not in this layer.  */
	if (!stil
	    || robust_scm2int (g->get_property ("layer"), 1) != i)
	  continue;

	Offset o (g->relative_coordinate (this, X_AXIS),
		  g->relative_coordinate (this, Y_AXIS));

	Offset extra = robust_scm2offset (g->get_property ("extra-offset"),
					  Offset (0, 0))
	  * Staff_symbol_referencer::staff_space (g);

	/* Must copy the stencil, for we cannot change the stencil
	   cached in G.  */

	Stencil st = *stil;
	st.translate (o + extra);

	*tail = scm_cons (st.expr (), SCM_EOL);
	tail = SCM_CDRLOC (*tail);
      }

  if (Stencil *me = get_stencil ())
    exprs = scm_cons (me->expr (), exprs);

  Interval x (extent (this, X_AXIS));
  Interval y (extent (this, Y_AXIS));
  Stencil sys_stencil (Box (x, y),
		       scm_cons (ly_symbol2scm ("combine-stencil"),
				 exprs));

  Interval staff_refpoints;
  staff_refpoints.set_empty ();
  extract_grob_set (this, "spaceable-staves", staves);
  for (int i = staves.size (); i--; )
    {
      Grob *g = staves[i];
      staff_refpoints.add_point (g->relative_coordinate (this, Y_AXIS));
    }

  Paper_system *pl = new Paper_system (sys_stencil, false);
  pl->staff_refpoints_ = staff_refpoints;
  Item *break_point = this->get_bound (LEFT);
  pl->break_before_penalty_
    = robust_scm2double (break_point->get_property ("page-penalty"), 0.0);

  return pl->unprotect ();
}

Link_array<Item>
System::broken_col_range (Item const *left, Item const *right) const
{
  Link_array<Item> ret;

  left = left->get_column ();
  right = right->get_column ();

  extract_grob_set (this, "columns", cols);
  int i = 0;
  while (i < cols.size()
	 && cols[i] != left)
    i++;

  if (i < cols.size())
    i ++;
  
  while (i < cols.size()
	 && cols[i] != right)
    {
      Paper_column *c = dynamic_cast<Paper_column *> (cols[i]);
      if (Item::is_breakable (c) && !c->system_)
	ret.push (c);
      i++;      
    }

  return ret;
}

/** Return all columns, but filter out any unused columns , since they might
    disrupt the spacing problem. */
Link_array<Grob>
System::columns () const
{
  extract_grob_set (this, "columns", ro_columns);

  int last_breakable = ro_columns.size ();

  while  (last_breakable --)
    {
      if (Item::is_breakable (ro_columns [last_breakable]))
	break;
    }

  Link_array<Grob> columns;
  for (int i = 0; i <= last_breakable; i++)
    {
      if (Paper_column::is_used (ro_columns[i]))
	columns.push (ro_columns[i]);
    }

  return columns;
}

int
System::get_rank () const
{
  return rank_;
}

ADD_INTERFACE (System, "system-interface",
	       "This is the toplevel object: each object in a score "
	       "ultimately has a System object as its X and Y parent. ",
	       "all-elements spaceable-staves columns")
