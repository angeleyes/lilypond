/*
  audio-column.cc -- implement Audio_column

  source file of the GNU LilyPond music typesetter

  (c) 1997 Jan Nieuwenhuizen <jan@digicash.com>
 */

#include "audio-column.hh"
#include "audio-item.hh"
#include "audio-score.hh"

#include "debug.hh"

Audio_column::Audio_column (Moment at_mom)
{
  at_mom_ = at_mom;
  audio_score_l_ = 0;
}

void
Audio_column::add (Audio_item* l)
{
  audio_item_l_list_.bottom().add (l);
  l->audio_column_l_ = this; 
}

Moment
Audio_column::at_mom() const
{
  return at_mom_;
}

void
Audio_column::print() const
{
#ifndef NPRINT
  DOUT << "Audio_column {";
  DOUT << "at: " << at_mom_ << "\n";
  DOUT << "}\n";
#endif 
}

