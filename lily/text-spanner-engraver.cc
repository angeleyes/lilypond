/*
  text-spanner-engraver.cc -- implement Text_spanner_engraver

  source file of the GNU LilyPond music typesetter

  (c) 2000--2007 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include "engraver.hh"

#include "international.hh"
#include "note-column.hh"
#include "side-position-interface.hh"
#include "stream-event.hh"

#include "translator.icc"

class Text_spanner_engraver : public Engraver
{
public:
  TRANSLATOR_DECLARATIONS (Text_spanner_engraver);
protected:
  virtual void finalize ();
  DECLARE_ACKNOWLEDGER (note_column);
  DECLARE_TRANSLATOR_LISTENER (text_span);
  void stop_translation_timestep ();
  void process_music ();

private:
  Spanner *span_;
  Spanner *finished_;
  Stream_event *current_event_;
  Drul_array<Stream_event *> event_drul_;
  void typeset_all ();
};

Text_spanner_engraver::Text_spanner_engraver ()
{
  finished_ = 0;
  current_event_ = 0;
  span_ = 0;
  event_drul_[START] = 0;
  event_drul_[STOP] = 0;
}

IMPLEMENT_TRANSLATOR_LISTENER (Text_spanner_engraver, text_span);
void
Text_spanner_engraver::listen_text_span (Stream_event *ev)
{
  Direction d = to_dir (ev->get_property ("span-direction"));
  ASSIGN_EVENT_ONCE (event_drul_[d], ev);
}

void
Text_spanner_engraver::process_music ()
{
  if (event_drul_[STOP])
    {
      if (!span_)
	event_drul_[STOP]->origin ()->warning (_ ("can't find start of text spanner"));
      else
	{
	  finished_ = span_;
	  span_ = 0;
	  current_event_ = 0;
	}
    }

  if (event_drul_[START])
    {
      if (current_event_)
	event_drul_[START]->origin ()->warning (_ ("already have a text spanner"));
      else
	{
	  current_event_ = event_drul_[START];
	  span_ = make_spanner ("TextSpanner", event_drul_[START]->self_scm ());

	  Side_position_interface::set_axis (span_, Y_AXIS);
	  event_drul_[START] = 0;
	}
    }
}

void
Text_spanner_engraver::acknowledge_note_column (Grob_info info)
{
  Spanner *spans[2] ={span_, finished_};
  for (int i = 0; i < 2; i++)
    {
      if (spans[i])
	{
	  Side_position_interface::add_support (spans[i], info.grob ());
	  add_bound_item (spans[i], dynamic_cast<Item *> (info.grob ()));
	}
    }
}

void
Text_spanner_engraver::typeset_all ()
{
  if (finished_)
    {
      if (!finished_->get_bound (RIGHT))
	{
	  Grob *e = unsmob_grob (get_property ("currentMusicalColumn"));
	  finished_->set_bound (RIGHT, e);
	}
      finished_ = 0;
    }
}

void
Text_spanner_engraver::stop_translation_timestep ()
{
  if (span_ && !span_->get_bound (LEFT))
    {
      Grob *e = unsmob_grob (get_property ("currentMusicalColumn"));
      span_->set_bound (LEFT, e);
    }

  typeset_all ();
  event_drul_[START] = 0;
  event_drul_[STOP] = 0;
}

void
Text_spanner_engraver::finalize ()
{
  typeset_all ();
  if (span_)
    {
      current_event_->origin ()->warning (_ ("unterminated text spanner"));
      span_->suicide ();
      span_ = 0;
    }
}

ADD_ACKNOWLEDGER (Text_spanner_engraver, note_column);
ADD_TRANSLATOR (Text_spanner_engraver,
		/* doc */ "Create text spanner from an event.",
		/* create */ "TextSpanner",
		/* read */ "",
		/* write */ "");
