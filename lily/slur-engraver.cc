/*
  slur-engraver.cc -- implement Slur_engraver

  source file of the GNU LilyPond music typesetter

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "slur.hh"
#include "note-column.hh"
#include "context.hh"
#include "directional-element-interface.hh"
#include "engraver.hh"
#include "spanner.hh"
#include "tie.hh"

/*
  It is possible that a slur starts and ends on the same note.  At
  least, it is for phrasing slurs: a note can be both beginning and
  ending of a phrase.
*/

class Slur_engraver : public Engraver
{
  Drul_array<Music *> events_;
  Music *running_slur_start_;
  Link_array<Grob> slurs_;
  Link_array<Grob> end_slurs_;

  void set_melisma (bool);

protected:
  virtual bool try_music (Music *);

  DECLARE_ACKNOWLEDGER (accidental);
  DECLARE_ACKNOWLEDGER (fingering);
  DECLARE_ACKNOWLEDGER (dynamic_line_spanner);
  DECLARE_ACKNOWLEDGER (note_column);
  DECLARE_ACKNOWLEDGER (script);
  DECLARE_ACKNOWLEDGER (text_script);
  DECLARE_ACKNOWLEDGER (tie);
  void acknowledge_extra_object (Grob_info);
  void stop_translation_timestep ();
  virtual void finalize ();
  void process_music ();

public:
  TRANSLATOR_DECLARATIONS (Slur_engraver);
};

Slur_engraver::Slur_engraver ()
{
  events_[START] = events_[STOP] = 0;
}

bool
Slur_engraver::try_music (Music *m)
{
  if (m->is_mus_type ("slur-event"))
    {
      Direction d = to_dir (m->get_property ("span-direction"));
      if (d == START)
	{
	  events_[START] = m;
	  return true;
	}
      else if (d == STOP)
	{
	  events_[STOP] = m;
	  return true;
	}
    }
  return false;
}

void
Slur_engraver::set_melisma (bool m)
{
  context ()->set_property ("slurMelismaBusy", m ? SCM_BOOL_T : SCM_BOOL_F);
}

void
Slur_engraver::acknowledge_note_column (Grob_info info)
{
  Grob *e = info.grob ();
  for (int i = slurs_.size (); i--;)
    Slur::add_column (slurs_[i], e);
  for (int i = end_slurs_.size (); i--;)
    Slur::add_column (end_slurs_[i], e);
}

void
Slur_engraver::acknowledge_extra_object (Grob_info info)
{
  Grob *e = info.grob ();
  SCM inside = e->get_property ("inside-slur");
  if (Tie::has_interface (e)
      || to_boolean (inside))
    {
      for (int i = slurs_.size (); i--;)
	Slur::add_extra_encompass (slurs_[i], e);
      for (int i = end_slurs_.size (); i--;)
	Slur::add_extra_encompass (end_slurs_[i], e);
    }
  else if (inside == SCM_BOOL_F
	   && e->name () != "DynamicText")
    {
      Grob *slur = slurs_.size () ? slurs_[0] : 0;
      slur = (end_slurs_.size () && !slur)
	? end_slurs_[0] : slur;

      if (slur)
	{
	  e->add_offset_callback (Slur::outside_slur_callback_proc, Y_AXIS);
	  e->set_object ("slur", slur->self_scm ());
	}
    }
}

void
Slur_engraver::acknowledge_accidental (Grob_info info)
{
  acknowledge_extra_object (info);
}

void
Slur_engraver::acknowledge_dynamic_line_spanner (Grob_info info)
{
  acknowledge_extra_object (info);
}

void
Slur_engraver::acknowledge_fingering (Grob_info info)
{
  acknowledge_extra_object (info);
}

void
Slur_engraver::acknowledge_script (Grob_info info)
{
  acknowledge_extra_object (info);
}

void
Slur_engraver::acknowledge_text_script (Grob_info info)
{
  acknowledge_extra_object (info);
}

void
Slur_engraver::acknowledge_tie (Grob_info info)
{
  acknowledge_extra_object (info);
}

void
Slur_engraver::finalize ()
{
  if (slurs_.size ())
    slurs_[0]->warning (_ ("unterminated slur"));
}

void
Slur_engraver::process_music ()
{
  if (events_[STOP])
    {
      if (slurs_.size () == 0)
	events_[STOP]->origin ()->warning (_ ("can't end slur"));

      end_slurs_ = slurs_;
      slurs_.clear ();
    }

  if (events_[START] && slurs_.is_empty ())
    {
      Music *ev = events_[START];

      bool double_slurs = to_boolean (get_property ("doubleSlurs"));

      Grob *slur = make_spanner ("Slur", events_[START]->self_scm ());
      Direction updown = to_dir (ev->get_property ("direction"));
      if (updown && !double_slurs)
	set_grob_direction (slur, updown);

      slurs_.push (slur);

      if (double_slurs)
	{
	  set_grob_direction (slur, DOWN);
	  slur = make_spanner ("Slur", events_[START]->self_scm ());
	  set_grob_direction (slur, UP);
	  slurs_.push (slur);
	}
    }
  set_melisma (slurs_.size ());
}

void
Slur_engraver::stop_translation_timestep ()
{
  end_slurs_.clear ();
  events_[START] = events_[STOP] = 0;
}

#include "translator.icc"

ADD_ACKNOWLEDGER (Slur_engraver, accidental);
ADD_ACKNOWLEDGER (Slur_engraver, dynamic_line_spanner);
ADD_ACKNOWLEDGER (Slur_engraver, fingering);
ADD_ACKNOWLEDGER (Slur_engraver, note_column);
ADD_ACKNOWLEDGER (Slur_engraver, script);
ADD_ACKNOWLEDGER (Slur_engraver, text_script);
ADD_ACKNOWLEDGER (Slur_engraver, tie);
ADD_TRANSLATOR (Slur_engraver,
		/* doc */ "Build slur grobs from slur events",
		/* create */ "Slur",
		/* accept */ "slur-event",
		/* read */ "slurMelismaBusy doubleSlurs",
		/* write */ "");
