/*
  phrasing-slur-engraver.cc -- implement Phrasing_slur_engraver

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "context.hh"
#include "directional-element-interface.hh"
#include "engraver.hh"
#include "slur.hh"
#include "note-column.hh"
#include "spanner.hh"
#include "tie.hh"

/*
  It is possible that a slur starts and ends on the same note.  At
  least, it is for phrasing slurs: a note can be both beginning and
  ending of a phrase.
*/

class Phrasing_slur_engraver : public Engraver
{
  Drul_array<Music *> events_;
  Music *running_slur_start_;
  Link_array<Grob> slurs_;
  Link_array<Grob> end_slurs_;

protected:
  virtual bool try_music (Music *);

  void acknowledge_extra_object (Grob_info);
  DECLARE_ACKNOWLEDGER(note_column);
  DECLARE_ACKNOWLEDGER(accidental);
  DECLARE_ACKNOWLEDGER(fingering);
  DECLARE_ACKNOWLEDGER(script);
  DECLARE_ACKNOWLEDGER(tie);
  DECLARE_ACKNOWLEDGER(text_script);
  DECLARE_ACKNOWLEDGER(slur);
  
  PRECOMPUTED_VIRTUAL void stop_translation_timestep ();
  virtual void finalize ();
  PRECOMPUTED_VIRTUAL void process_music ();

public:
  TRANSLATOR_DECLARATIONS (Phrasing_slur_engraver);
};

Phrasing_slur_engraver::Phrasing_slur_engraver ()
{
  events_[START] = events_[STOP] = 0;
}

bool
Phrasing_slur_engraver::try_music (Music *m)
{
  if (m->is_mus_type ("phrasing-slur-event"))
    {
      /*
	Let's not start more than one slur per moment.
      */
      Direction d = to_dir (m->get_property ("span-direction"));
      if (d == START)
	{
	  events_[START] = m;
	  return true;
	}
      else if (d == STOP)
	{
	  if (slurs_.is_empty ())
	    return false;

	  events_[STOP] = m;
	  return true;
	}
    }
  return false;
}

void
Phrasing_slur_engraver::acknowledge_note_column (Grob_info info)
{
  Grob *e = info.grob ();
  for (int i = slurs_.size (); i--;)
    Slur::add_column (slurs_[i], e);
  for (int i = end_slurs_.size (); i--;)
    Slur::add_column (end_slurs_[i], e);
}

void
Phrasing_slur_engraver::acknowledge_extra_object (Grob_info info)
{
  Grob*e = info.grob ();
  SCM inside = e->get_property ("inside-slur");
  if (Tie::has_interface (e)
      || to_boolean (inside))
    {
      for (int i = slurs_.size (); i--;)
	Slur::add_extra_encompass (slurs_[i], e);
      for (int i = end_slurs_.size (); i--;)
	Slur::add_extra_encompass (end_slurs_[i], e);
    }
  else if (inside == SCM_BOOL_F)
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
Phrasing_slur_engraver::acknowledge_accidental (Grob_info info)
{
  acknowledge_extra_object (info);
}


void
Phrasing_slur_engraver::acknowledge_fingering (Grob_info info)
{
  acknowledge_extra_object (info);
}

void
Phrasing_slur_engraver::acknowledge_script (Grob_info info)
{
  acknowledge_extra_object (info);
}

void
Phrasing_slur_engraver::acknowledge_text_script (Grob_info info)
{
  acknowledge_extra_object (info);
}

void
Phrasing_slur_engraver::acknowledge_tie (Grob_info info)
{
  acknowledge_extra_object (info);
}


void
Phrasing_slur_engraver::acknowledge_slur (Grob_info info)
{
  acknowledge_extra_object (info);
}

void
Phrasing_slur_engraver::finalize ()
{
  if (slurs_.size ())
    slurs_[0]->warning (_ ("unterminated phrasing slur"));
}

void
Phrasing_slur_engraver::process_music ()
{
  if (events_[STOP])
    {
      end_slurs_ = slurs_;
      slurs_.clear ();
    }

  if (events_[START] && slurs_.is_empty ())
    {
      Music *ev = events_[START];

      Grob *slur = make_spanner ("PhrasingSlur", events_[START]->self_scm ());
      Direction updown = to_dir (ev->get_property ("direction"));
      if (updown)
	set_grob_direction (slur, updown);

      slurs_.push (slur);
    }
}

void
Phrasing_slur_engraver::stop_translation_timestep ()
{
  end_slurs_.clear ();
  events_[START] = events_[STOP] = 0;
}

#include "translator.icc"


ADD_ACKNOWLEDGER(Phrasing_slur_engraver,note_column);
ADD_ACKNOWLEDGER(Phrasing_slur_engraver,accidental);
ADD_ACKNOWLEDGER(Phrasing_slur_engraver,fingering)
ADD_ACKNOWLEDGER(Phrasing_slur_engraver,script);
ADD_ACKNOWLEDGER(Phrasing_slur_engraver,tie);
ADD_ACKNOWLEDGER(Phrasing_slur_engraver,text_script);
ADD_ACKNOWLEDGER(Phrasing_slur_engraver,slur);

ADD_TRANSLATOR (Phrasing_slur_engraver,
		/* descr */ "Print phrasing slurs. Similar to @ref{Slur_engraver}",
		/* creats*/ "PhrasingSlur",
		/* accepts */ "phrasing-slur-event",
		/* acks  */ "",
		/* reads */ "",
		/* write */ "");
