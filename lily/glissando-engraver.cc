/*
  note-head-line-engraver.cc -- implement Note_head_line_engraver

  source file of the GNU LilyPond music typesetter

  (c) 2000--2005 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include "warn.hh"
#include "spanner.hh"
#include "rhythmic-head.hh"
#include "engraver.hh"

/**
   Create line-spanner grobs for glissandi lines that connect note
   heads.
*/
class Glissando_engraver : public Engraver
{
public:
  TRANSLATOR_DECLARATIONS (Glissando_engraver);

protected:
  virtual void acknowledge_grob (Grob_info);
  virtual void finalize ();
  virtual void stop_translation_timestep ();
  virtual bool try_music (Music *);
  virtual void process_music ();
private:
  Spanner *line_;
  Spanner *last_line_;
  Music *event_;
};

Glissando_engraver::Glissando_engraver ()
{
  last_line_ = line_ = 0;
  event_ = 0;
}

bool
Glissando_engraver::try_music (Music *m)
{
  if (!event_)
    {
      event_ = m;
      return true;
    }
  return false;
}

void
Glissando_engraver::process_music ()
{
  if (event_)
    {
      line_ = make_spanner ("Glissando", event_->self_scm ());
    }
}

void
Glissando_engraver::acknowledge_grob (Grob_info info)
{
  if (Rhythmic_head::has_interface (info.grob ()))
    {
      Grob *g = info.grob ();
      if (line_)
	line_->set_bound (LEFT, g);

      if (last_line_)
	last_line_->set_bound (RIGHT, g);
    }
}

void
Glissando_engraver::stop_translation_timestep ()
{
  if (last_line_ && last_line_->get_bound (RIGHT))
    {
      last_line_ = 0;
    }
  if (line_)
    {
      if (last_line_)
	programming_error ("overwriting glissando");
      last_line_ = line_;
    }
  line_ = 0;
  event_ = 0;
}

void
Glissando_engraver::finalize ()
{
  if (line_)
    {
      String msg = _ ("unterminated glissando");

      if (event_)
	event_->origin ()->warning (msg);
      else
	warning (msg);

      line_->suicide ();
      line_ = 0;
    }
}

ADD_TRANSLATOR (Glissando_engraver,
		/* descr */ "Engrave a glissandi",
		/* creats*/ "Glissando",
		/* accepts */ "glissando-event",
		/* acks  */ "rhythmic-head-interface",
		/* reads */ "followVoice",
		/* write */ "");
