/*
  slur-performer.cc -- implement Slur_performer

  source file of the GNU LilyPond music typesetter

  (c) 1996--2004 Jan Nieuwenhuizen <janneke@gnu.org>
 */

#include "performer.hh"
#include "event.hh"
#include "audio-item.hh"
#include "audio-column.hh"
#include "global-context.hh"
#include "warn.hh"

/*
  this is C&P from beam_performer.
 */

class Slur_performer : public Performer {
public:
  TRANSLATOR_DECLARATIONS(Slur_performer);
  
protected:
  virtual bool try_music (Music *ev) ;
  virtual void start_translation_timestep ();
  virtual void process_music ();
  void set_melisma (bool);
private:
  Music *start_ev_;
  Music *now_stop_ev_;
  bool slur_;
};

void 
Slur_performer::process_music ()
{
  if (now_stop_ev_)
    {
      slur_ = false;
    }

  if (start_ev_)
    {
      slur_ = true;
      set_melisma (true);
    }
}


void
Slur_performer::set_melisma (bool ml)
{
  daddy_context_->set_property ("slurMelismaBusy", ml ? SCM_BOOL_T :SCM_BOOL_F);
}

void
Slur_performer::start_translation_timestep ()
{
  if (slur_)
    {
      set_melisma (true);
    }
  
  start_ev_ = 0;
  now_stop_ev_ = 0;
}
 
bool
Slur_performer::try_music (Music *m)
{
  if (m->is_mus_type ("slur-event"))
    {
      Direction d = to_dir (m->get_mus_property ("span-direction"));

      if (d == START)
	{
	  start_ev_ = m;
	}
      else if (d==STOP)
	{
	  now_stop_ev_ = m;
	}
      return true;
    }
  return false;
}

ENTER_DESCRIPTION(Slur_performer,"","",
		  "slur-event","","","");

Slur_performer::Slur_performer()
{
  slur_ = false;
}
