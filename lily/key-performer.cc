/*
  key-performer.cc -- implement Key_performer

  source file of the GNU LilyPond music typesetter

  (c)  1997--2003 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include "lily-guile.hh"

#include "audio-item.hh"
#include "performer.hh"
#include "warn.hh"


class Key_performer : public Performer
{
public:
  TRANSLATOR_DECLARATIONS(Key_performer);
  ~Key_performer ();

protected:
  virtual bool try_music (Music* req);
  virtual void create_audio_elements ();
  virtual void stop_translation_timestep ();

private:
  Key_change_ev* key_req_;
  Audio_key* audio_;
};

Key_performer::Key_performer ()
{
  key_req_ = 0;
  audio_ = 0;
}

Key_performer::~Key_performer ()
{
}

void
Key_performer::create_audio_elements ()
{
  if (key_req_) 
    {

      /*
	UGH. primitive-eval.
       */
      SCM pitchlist = key_req_->get_mus_property ("pitch-alist");
      SCM proc = scm_primitive_eval (ly_symbol2scm ("accidentals-in-key")); 
      SCM acc = gh_call1 (proc, pitchlist);
 
      Pitch my_do (0, 
		   gh_scm2int (ly_caar (pitchlist)),
		   gh_scm2int (ly_cdar (pitchlist)));
		  
      Pitch to_c (-1,
		   (7 - gh_scm2int (ly_caar (pitchlist))) % 7,
		   -gh_scm2int (ly_cdar (pitchlist)));

      my_do = my_do.transposed (to_c);
      to_c = my_do.transposed (Pitch(0,0,- my_do.get_alteration ()));

      SCM c_pitchlist = transpose_key_alist (pitchlist, to_c.smobbed_copy());

      /*
	MIDI keys are too limited for lilypond scales.

	TODO: should probably detect minor key, though.
      */
      audio_ = new Audio_key (gh_scm2int (acc), true); 
      Audio_element_info info (audio_, key_req_);
      announce_element (info);
      key_req_ = 0;
    }
}

void
Key_performer::stop_translation_timestep ()
{
  if (audio_)
    {
      play_element (audio_);
      audio_ = 0;
    }
}

bool
Key_performer::try_music (Music* req)
{
  if (Key_change_ev *kc = dynamic_cast <Key_change_ev *> (req))
    {
      if (key_req_)
	warning (_ ("FIXME: key change merge"));

      key_req_ = kc;
      return true;
    }

  return false;
}

ENTER_DESCRIPTION(Key_performer,
		  "","",
		  "key-change-event",
		  "","","");
