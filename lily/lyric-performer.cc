/*
  lyric-performer.cc -- implement Lyric_performer

  source file of the GNU LilyPond music typesetter

  (c) 1997--2005 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include "audio-item.hh"
#include "performer.hh"
#include "music.hh"

class Lyric_performer : public Performer
{
public:
  TRANSLATOR_DECLARATIONS (Lyric_performer);
protected:

  virtual bool try_music (Music *event);
  PRECOMPUTED_VIRTUAL void stop_translation_timestep ();
  virtual void create_audio_elements ();

private:
  Link_array<Music> events_;
  Audio_text *audio_;
};

Lyric_performer::Lyric_performer ()
{
  audio_ = 0;
}

void
Lyric_performer::create_audio_elements ()
{
  // FIXME: won't work with fancy lyrics
  if (events_.size ()
      && scm_is_string (events_[0]->get_property ("text"))
      && ly_scm2string (events_[0]->get_property ("text")).length ())
    {
      audio_ = new Audio_text (Audio_text::LYRIC,
			       ly_scm2string (events_[0]->get_property ("text")));
      Audio_element_info info (audio_, events_[0]);
      announce_element (info);
    }
  events_.clear ();
}

void
Lyric_performer::stop_translation_timestep ()
{
  if (audio_)
    {
      play_element (audio_);
      audio_ = 0;
    }
  events_.clear ();
}

bool
Lyric_performer::try_music (Music *event)
{
  if (event->is_mus_type ("lyric-event"))
    {
      events_.push (event);
      return true;
    }
  return false;
}

#include "translator.icc"

ADD_TRANSLATOR (Lyric_performer, "", "", "lyric-event", "", "", "");
