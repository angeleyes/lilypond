/*
  timing-grav.cc -- implement Timing_engraver

  source file of the GNU LilyPond music typesetter

  (c) 1997 Han-Wen Nienhuys <hanwen@stack.nl>
*/
#include "score-grav.hh"
#include "timing-grav.hh"
#include "command-request.hh"



void
Timing_engraver::fill_staff_info (Staff_info &inf)
{
  inf.time_C_ = &time_;
  inf.rhythmic_C_ = &default_grouping_;
}

IMPLEMENT_IS_TYPE_B1(Timing_engraver, Timing_translator);
ADD_THIS_TRANSLATOR(Timing_engraver);
