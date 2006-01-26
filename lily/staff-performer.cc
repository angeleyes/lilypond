/*
  staff-performer.cc -- implement Staff_performer

  source file of the GNU LilyPond music typesetter

  (c) 1997--2006 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include "warn.hh"
#include "audio-column.hh"
#include "audio-item.hh"
#include "audio-staff.hh"
#include "performer-group.hh"
#include "context.hh"

/** Perform a staff. Individual notes should have their instrument
    (staff-wide) set, so we override play_element ()
*/
class Staff_performer : public Performer
{
public:
  TRANSLATOR_DECLARATIONS (Staff_performer);
  ~Staff_performer ();

  std::string new_instrument_string ();
  std::string instrument_string_;

protected:
  virtual void acknowledge_audio_element (Audio_element_info info);
  virtual void finalize ();
  virtual void initialize ();
  void process_music ();
  void stop_translation_timestep ();

private:
  Audio_staff *audio_staff_;
  Audio_instrument *instrument_;
  Audio_text *instrument_name_;
  Audio_text *name_;
  Audio_tempo *tempo_;
};

#include "translator.icc"

ADD_TRANSLATOR (Staff_performer, "", "",
		"",
		"", "");

Staff_performer::Staff_performer ()
{
  audio_staff_ = 0;
  instrument_ = 0;
  instrument_name_ = 0;
  name_ = 0;
  tempo_ = 0;
}

Staff_performer::~Staff_performer ()
{
}

void
Staff_performer::initialize ()
{
  audio_staff_ = new Audio_staff;
  name_ = new Audio_text (Audio_text::TRACK_NAME, context ()->id_string ());
  tempo_ = new Audio_tempo (get_tempo ());

  audio_staff_->add_audio_item (name_);
  audio_staff_->add_audio_item (tempo_);
  
  announce_element (Audio_element_info (audio_staff_, 0));
  announce_element (Audio_element_info (name_, 0));
  announce_element (Audio_element_info (tempo_, 0));
}

void
Staff_performer::process_music ()
{
  std::string str = new_instrument_string ();
  if (str.length ())
    {
      instrument_name_ = new Audio_text (Audio_text::INSTRUMENT_NAME, str);
      announce_element (Audio_element_info (instrument_name_, 0));
      instrument_ = new Audio_instrument (str);
      announce_element (Audio_element_info (instrument_, 0));

      audio_staff_->add_audio_item (instrument_);
      audio_staff_->add_audio_item (instrument_name_);
     
      /*
	Have to be here before notes arrive into the staff.
      */
      play_element (instrument_);
      play_element (instrument_name_);
    }
}

void
Staff_performer::stop_translation_timestep ()
{
  SCM proc = ly_lily_module_constant ("percussion?");

  SCM drums = scm_call_1 (proc, ly_symbol2scm (instrument_string_.c_str ()));
  audio_staff_->channel_ = (drums == SCM_BOOL_T ? 9 : -1);
  if (name_)
    {
      play_element (name_);
      name_ = 0;
    }
  if (tempo_)
    {
      play_element (tempo_);
      tempo_ = 0;
    }
  instrument_name_ = 0;
  instrument_ = 0;
}

void
Staff_performer::finalize ()
{
  Performer::play_element (audio_staff_);
  audio_staff_ = 0;
}

std::string
Staff_performer::new_instrument_string ()
{
  // mustn't ask Score for instrument: it will return piano!
  SCM minstr = get_property ("midiInstrument");

  if (!scm_is_string (minstr)
      || ly_scm2string (minstr) == instrument_string_)
    return "";

  instrument_string_ = ly_scm2string (minstr);

  return instrument_string_;
}

void
Staff_performer::acknowledge_audio_element (Audio_element_info inf)
{
  if (Audio_item *ai = dynamic_cast<Audio_item *> (inf.elem_))
    audio_staff_->add_audio_item (ai);
}

