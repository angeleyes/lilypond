/*
  tempo-performer.cc -- implement Tempo_performer

  source file of the GNU LilyPond music typesetter

  (c) 1997--2005 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include "performer.hh"

#include "audio-item.hh"
#include "music.hh"
#include "duration.hh"

class Tempo_performer : public Performer
{
public:
  TRANSLATOR_DECLARATIONS (Tempo_performer);
  ~Tempo_performer ();

protected:

  virtual bool try_music (Music *req);
  virtual void stop_translation_timestep ();
  virtual void create_audio_elements ();

private:
  Music *tempo_req_;
  Audio_tempo *audio_;
};

Tempo_performer::Tempo_performer ()
{
  tempo_req_ = 0;
  audio_ = 0;
}

Tempo_performer::~Tempo_performer ()
{
}

void
Tempo_performer::create_audio_elements ()
{
  if (tempo_req_)
    {
      SCM met = tempo_req_->get_property ("metronome-count");
      Duration *d = unsmob_duration (tempo_req_->get_property ("tempo-unit"));

      Rational r = (d->get_length () / Moment (Rational (1, 4)) * Moment (scm_to_int (met))).main_part_;

      audio_ = new Audio_tempo (int (r));

      Audio_element_info info (audio_, tempo_req_);
      announce_element (info);
      tempo_req_ = 0;
    }
}

void
Tempo_performer::stop_translation_timestep ()
{
  if (audio_)
    {
      play_element (audio_);
      audio_ = 0;
    }
}

bool
Tempo_performer::try_music (Music *req)
{
  if (tempo_req_)
    return false;

  tempo_req_ = req;
  return true;
}

ADD_TRANSLATOR (Tempo_performer, "", "",
		"metronome-change-event",
		"", "", "");
