/*
  spanner.cc -- implement Spanner

  source file of the GNU LilyPond music typesetter

  (c) 1996,1997 Han-Wen Nienhuys <hanwen@stack.nl>
*/

#include "debug.hh"
#include "spanner.hh"
#include "p-col.hh"
#include "p-score.hh"


IMPLEMENT_IS_TYPE_B1(Spanner,Score_elem);

void
Spanner::do_print() const
{
#ifndef NPRINT
  DOUT << "Between col ";
  if (broken_into_l_arr_.size())
    DOUT << "with broken pieces\n";
#endif
}

void
Spanner::break_into_pieces ()
{
  if (broken_into_l_arr_.size())
    return; 
	 
  Item * left = spanned_drul_[LEFT];
  Item * right = spanned_drul_[RIGHT];
  
  
  Link_array<Item> break_cols = pscore_l_->broken_col_range (left,right);
  Link_array<Spanner> broken_into_l_arr;

  break_cols.insert (left,0);
  break_cols.push (right);

  for (int i=1; i < break_cols.size(); i++) 
    {
      Spanner* span_p = clone()->spanner ();
      left = break_cols[i-1];
      right = break_cols[i];
      if (!right->line_l())
	right = right->find_prebroken_piece(-1);
      if (!left->line_l())
	left = left->find_prebroken_piece(1);

            assert (left&&right && left->line_l() == right->line_l());

      span_p->set_bounds(LEFT,left);
      span_p->set_bounds(RIGHT,right);
	
      pscore_l_->typeset_broken_spanner (span_p);
      broken_into_l_arr.push (span_p);
    }
   
  broken_into_l_arr_ = broken_into_l_arr;
}

void
Spanner::set_my_columns()
{
  Direction i = (Direction)1;
  do 
    {
      if (!spanned_drul_[i]->line_l())
	set_bounds(i,spanned_drul_[i]->find_prebroken_piece(-i));
    } 
  while ((i*=-1) != 1);
}       


void
Spanner::set_bounds(Direction d, Item*i)
{
  if (spanned_drul_[d])
    spanned_drul_[d]->attached_span_l_arr_.substitute(this,0);
  
  spanned_drul_[d] =i;
  if (i)
    i->attached_span_l_arr_.push(this);
}

void
Spanner::do_break_processing()
{
  if (!line_l())
    {
      break_into_pieces ();
      for (int i=0; i < broken_into_l_arr_.size(); i++)
	broken_into_l_arr_[i]->handle_broken_dependencies();
    }
  else 
    {
      handle_broken_dependencies();
    }
}


Spanner::Spanner()
{
  spanned_drul_[LEFT]=0;
  spanned_drul_[RIGHT]=0;
}

String
Spanner::do_TeX_output_str () const
{
  Offset left_off (spanned_drul_[LEFT]->absolute_coordinate(X_AXIS), 0);
  return make_TeX_string (absolute_offset() + left_off);
}

Interval
Spanner::do_width() const
{
  Real l = spanned_drul_[LEFT]->absolute_coordinate (X_AXIS);
  Real r = spanned_drul_[RIGHT]->absolute_coordinate (X_AXIS);
  assert (r>=l);
	
  return Interval (0, r-l);
}

Line_of_score *
Spanner::line_l() const
{
  if (!spanned_drul_[LEFT] || !spanned_drul_[RIGHT])
    return 0;
  if (spanned_drul_[LEFT]->line_l() != spanned_drul_[RIGHT]->line_l())
    return 0;
  return spanned_drul_[LEFT]->line_l();
}


Spanner*
Spanner::find_broken_piece (Line_of_score*l) const
{
  for (int i=0; i < broken_into_l_arr_.size(); i++)
    if (broken_into_l_arr_[i]->line_l() == l)
      return broken_into_l_arr_[i];
  return 0;				   
	  
}

bool
Spanner::broken_b() const
{
  return broken_into_l_arr_.size();
}

void
Spanner::do_unlink() 
{
  if (spanned_drul_[LEFT]) 
    {
      spanned_drul_[LEFT]->attached_span_l_arr_.substitute (this,0);
      spanned_drul_[LEFT] =0;
    }
  if (spanned_drul_[RIGHT]) 
    {
      spanned_drul_[RIGHT]->attached_span_l_arr_.substitute (this,0);
      spanned_drul_[RIGHT] = 0;
    }
}

void
Spanner::do_junk_links()
{
  spanned_drul_[LEFT] = spanned_drul_[RIGHT] =0;
}
