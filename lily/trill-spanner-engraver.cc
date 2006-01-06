/*
  trill-spanner-engraver.cc -- implement Trill_spanner_engraver

  source file of the GNU LilyPond music typesetter

  (c) 2000--2006 Jan Nieuwenhuizen <janneke@gnu.org>
*/

/*
  C&P from text-spanner.cc

  - todo: ending should be detected automatically? a new note
  automatically is the end of the trill?
*/

#include "note-column.hh"
#include "side-position-interface.hh"
#include "engraver.hh"

#include "translator.icc"

class Trill_spanner_engraver : public Engraver
{
public:
  TRANSLATOR_DECLARATIONS (Trill_spanner_engraver);
protected:
  virtual void finalize ();
  DECLARE_ACKNOWLEDGER (note_column);
  virtual bool try_music (Music *);
  void stop_translation_timestep ();
  void process_music ();

private:
  Spanner *span_;
  Spanner *finished_;
  Music *current_event_;
  Drul_array<Music *> event_drul_;
  void typeset_all ();
};

Trill_spanner_engraver::Trill_spanner_engraver ()
{
  finished_ = 0;
  current_event_ = 0;
  span_ = 0;
  event_drul_[START] = 0;
  event_drul_[STOP] = 0;
}

bool
Trill_spanner_engraver::try_music (Music *m)
{
  if (m->is_mus_type ("trill-span-event"))
    {
      Direction d = to_dir (m->get_property ("span-direction"));
      event_drul_[d] = m;
      return true;
    }

  return false;
}

void
Trill_spanner_engraver::process_music ()
{
  if (event_drul_[STOP])
    {
      if (!span_)
	event_drul_[STOP]->origin ()->warning (_ ("can't find start of trill spanner"));
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
	event_drul_[START]->origin ()->warning (_ ("already have a trill spanner"));
      else
	{
	  current_event_ = event_drul_[START];
	  span_ = make_spanner ("TrillSpanner", event_drul_[START]->self_scm ());
	  Side_position_interface::set_axis (span_, Y_AXIS);
	  event_drul_[START] = 0;
	}
    }
}

void
Trill_spanner_engraver::acknowledge_note_column (Grob_info info)
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
Trill_spanner_engraver::typeset_all ()
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
Trill_spanner_engraver::stop_translation_timestep ()
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
Trill_spanner_engraver::finalize ()
{
  typeset_all ();
  if (span_)
    {
      current_event_->origin ()->warning (_ ("unterminated trill spanner"));
      span_->suicide ();
      span_ = 0;
    }
}
ADD_ACKNOWLEDGER (Trill_spanner_engraver, note_column);
ADD_TRANSLATOR (Trill_spanner_engraver,
		/* doc */ "Create trill spanner from a Music.",
		/* create */ "TrillSpanner",
		/* accept */ "trill-span-event",
		/* read */ "",
		/* write */ "");
