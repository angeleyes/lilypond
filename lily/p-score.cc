/*
  p-score.cc -- implement Paper_score

  source file of the GNU LilyPond music typesetter

  (c) 1996, 1997 Han-Wen Nienhuys <hanwen@stack.nl>
*/

#include "super-elem.hh"
#include "debug.hh"
#include "lookup.hh"
#include "spanner.hh"
#include "paper-def.hh"
#include "scoreline.hh"
#include "p-score.hh"
#include "tex-stream.hh"
#include "p-col.hh"
#include "header.hh"
#include "word-wrap.hh"
#include "gourlay-breaking.hh"

Paper_score::Paper_score ()
{
  super_elem_l_   = new Super_elem;
  typeset_element (super_elem_l_);
}

Paper_score::~Paper_score ()
{
  super_elem_l_->unlink_all ();
  for (PCursor<Score_elem*> i(elem_p_list_.top()); i.ok(); i++)
    assert(!i->linked_b());
}

void
Paper_score::typeset_element (Score_elem * elem_p)
{
  elem_p_list_.bottom ().add (elem_p);
  elem_p->pscore_l_ = this;

  elem_p->add_processing ();
}

void
Paper_score::typeset_broken_spanner (Spanner*span_p)
{
  typeset_element (span_p);
}


void
Paper_score::typeset_unbroken_spanner (Spanner*span_p)
{
  span_p_list_.bottom ().add (span_p);
  span_p->pscore_l_=this;

  
  // do not init start/stop fields. These are for broken spans only.
  span_p->add_processing ();
}


void
Paper_score::clean_cols ()
{
  int rank_i = 0;
  for (iter_top (col_p_list_,c); c.ok ();)
    {
      c->set_rank (rank_i++);
      c++;
    }
}

void
Paper_score::add_column (Paper_column *p)
{
  col_p_list_.bottom ().add (p);
  typeset_element(p);
}



void
Paper_score::OK () const
{
#ifndef NDEBUG
  for (PCursor<Score_elem*> i (elem_p_list_.top ()); i.ok  (); i++) 
    i->OK ();
#endif
}

void
Paper_score::print () const
{    
#ifndef NPRINT
  if (!check_debug)
    return ;
  DOUT << "Paper_score { ";
  DOUT << "\n elements: ";
  for (iter_top (elem_p_list_,cc); cc.ok (); cc++)	
    cc->print ();
  DOUT << "\n unbroken spanners: ";
  for (iter (span_p_list_.top (), i); i.ok  (); i++)
    i->print ();
  
  DOUT << "}\n";
#endif 
}

void
Paper_score::preprocess ()
{
  super_elem_l_->breakable_col_processing ();
  super_elem_l_->pre_processing ();
}

void
Paper_score::postprocess ()
{
  super_elem_l_->post_processing ();
  super_elem_l_->molecule_processing ();
}

PCursor<Paper_column *>
Paper_score::find_col (Paper_column const *c) const
{
  Paper_column const *what = c;
  
  return col_p_list_.find ((Paper_column*)what);
}


void
Paper_score::set_breaking (Array<Col_hpositions> const &breaking)
{
  super_elem_l_->line_of_score_l_->set_breaking (breaking);
  super_elem_l_->break_processing ();


  for (iter (span_p_list_.top (),i); i.ok  ();) 
    {
      Spanner *span_p = i.remove_p ();
      if (span_p->broken_b ()) 
	{
	  span_p->unlink ();
	  delete span_p;
	}else
	  {
	    typeset_broken_spanner (span_p);
	  }
    }
  for (iter (elem_p_list_.top (),i); i.ok  () ;) 
    {
      Item *i_l =i->item ();
      if (i_l && !i_l->line_l ()) 
	{
	  i_l->unlink ();
	  delete i.remove_p ();
	}
      else
	i++;
    }
}

void
Paper_score::calc_breaking ()
{
  Break_algorithm *algorithm_p=0;
  Array<Col_hpositions> sol;
  bool try_wrap = ! paper_l_->get_var ("castingalgorithm");

  if (!try_wrap) 
    {
      algorithm_p = new Gourlay_breaking ;
      algorithm_p->set_pscore (this);
      sol = algorithm_p->solve ();
      delete algorithm_p;
      if (! sol.size ()) 
	{
	  warning ("Can not solve this casting problem exactly; revert to Word_wrap");
	  try_wrap = true;
	}
    }
  if  (try_wrap) 
    {
      algorithm_p = new Word_wrap;    
      algorithm_p->set_pscore (this);
      sol = algorithm_p->solve ();
      delete algorithm_p;
    }
  set_breaking (sol);
}

void
Paper_score::process ()
{
  clean_cols ();
  print ();
  *mlog << "Preprocessing elements... " <<flush;
  preprocess ();
  *mlog << "\nCalculating column positions ... " <<flush;
  calc_breaking ();
  *mlog << "\nPostprocessing elements..." << endl;
  postprocess ();

  // output
  String outname = paper_l_->outfile_str_ ;
  if (outname == "")
    outname = default_out_str_+ ".tex";
  
  *mlog << "TeX output to " <<  outname << " ...\n";
  
  Tex_stream the_output (outname);
  
  the_output << "% outputting Score, defined at: " << origin_str_ << "\n";
  if (header_l_) 
    {
      the_output << header_l_->TeX_string();
    }
  
  the_output << "\n "<<  paper_l_->lookup_l ()->texsetting << "%(Tex id)\n";
  the_output<< super_elem_l_->TeX_output_str ();
  the_output << "\n\\EndLilyPondOutput";
}

/** Get all breakable columns between l and r, (not counting l and r).  */
Link_array<Paper_column>
Paper_score::breakable_col_range (Paper_column*l,Paper_column*r) const
{
  Link_array<Paper_column> ret;

  PCursor<Paper_column*> start (l ? find_col (l)+1 : col_p_list_.top ());
  PCursor<Paper_column*> stop (r ? find_col (r) : col_p_list_.bottom ());

  /*
    ugh! windows-suck-suck-suck.
    */
  while (PCursor<Paper_column*>::compare (start,stop) < 0) 
    {
      if (start->breakable_b_)
	ret.push (start);
      start++;
    }

  return ret;
}
Link_array<Paper_column>
Paper_score::col_range (Paper_column*l, Paper_column*r) const
{
  Link_array<Paper_column> ret;
  
  PCursor<Paper_column*> start (l ? find_col (l)+1 : col_p_list_.top ());
  PCursor<Paper_column*> stop (r ? find_col (r) : col_p_list_.bottom ());
  ret.push (l);
  
  /*
    ugh! windows-suck-suck-suck.
    */
  while (PCursor<Paper_column*>::compare (start,stop) < 0)
    ret.push (start++);
  ret.push (r);
  return ret;
}

Link_array<Item>
Paper_score::broken_col_range (Item const*l_item_l, Item const*r_item_l) const
{
  Link_array<Item> ret;
  Item const*l=l_item_l;
  Item const*r=r_item_l;
  
  while (! l->is_type_b(Paper_column::static_name ())) 
    l = l->axis_group_l_a_[X_AXIS]->item ();
  
  while (! r->is_type_b(Paper_column::static_name ())) 
    r = r->axis_group_l_a_[X_AXIS]->item ();
   
  PCursor<Paper_column*> start (l ? find_col ((Paper_column*)l)+1 : col_p_list_.top ());
  PCursor<Paper_column*> stop (r ? find_col ((Paper_column*)r) : col_p_list_.bottom ());
  
  /*
    ugh! windows-suck-suck-suck.
    */
  while (PCursor<Paper_column*>::compare (start,stop) < 0) 
    {
      if (start->breakable_b_ && !start->line_l_)
	ret.push (start);
      start++;
    }

  return ret;
}
