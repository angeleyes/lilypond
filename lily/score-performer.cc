/*
  score-performer.cc -- implement Score_performer

  source file of the GNU LilyPond music typesetter

  (c) 1996, 1997 Jan Nieuwenhuizen <jan@digicash.com>
 */

#include "score-performer.hh"
#include "midi-def.hh"
#include "audio-column.hh"
#include "audio-item.hh"
#include "performance.hh"
#include "midi-stream.hh"
#include "string-convert.hh"
#include "debug.hh"
#include "score.hh"
#include "source-file.hh"
#include "source.hh"
#include "audio-staff.hh"

IMPLEMENT_IS_TYPE_B1(Score_performer,Performer_group_performer);
ADD_THIS_TRANSLATOR(Score_performer);


Score_performer::Score_performer()
{
}


Score_performer::~Score_performer()
{
}

void
Score_performer::play (Audio_element * p)
{
  if  (p->is_type_b (Audio_item::static_name())) 
    {
      audio_column_l_->add ((Audio_item*)p);
    }
  else if (p->is_type_b (Audio_staff::static_name())) 
    {
      performance_p_->add_staff ((Audio_staff*)p);
    }
  performance_p_->add (p);
}

void 
Score_performer::prepare (Moment m)
{
  Global_translator::prepare (m);
  audio_column_l_ = new Audio_column (m);
  performance_p_->add (audio_column_l_);
  post_move_processing ();
}


void 
Score_performer::process()
{
  process_requests();
  pre_move_processing();
  check_removal();
}

void
Score_performer::start()
{
}


int
Score_performer::get_tempo_i() const
{
  return performance_p_->midi_l_->get_tempo_i (Moment (1, 4));
}

void
Score_performer::finish()
{
  check_removal ();
  removal_processing();
}

Music_output *
Score_performer::get_output_p ()
{
  Music_output * o = performance_p_;
  performance_p_ =0;
  return o;
}

void
Score_performer::do_add_processing ()
{
  Translator_group::do_add_processing ();
  assert (output_def_l_->is_type_b (Midi_def::static_name ()));
  performance_p_ = new Performance;
  performance_p_->midi_l_ = (Midi_def*) output_def_l_;  
}
