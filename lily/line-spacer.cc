/*
  line-spacer.cc -- implement 

  source file of the GNU LilyPond music typesetter

  (c) 1997 Han-Wen Nienhuys <hanwen@stack.nl>
*/


#include "line-spacer.hh"

Line_spacer::Line_spacer()
{
  paper_l_ =0;
}
Paper_def*
Line_spacer::paper_l()const
{ 
  return paper_l_ ;
}
