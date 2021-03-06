/*
  This file is part of LilyPond, the GNU music typesetter.

  Copyright (C) 1997--2009 Jan Nieuwenhuizen <janneke@gnu.org>

  LilyPond is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  LilyPond is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with LilyPond.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "audio-item.hh"
#include "music-sequence.hh"
#include "performer.hh"
#include "stream-event.hh"
#include "warn.hh"

#include "translator.icc"

class Key_performer : public Performer
{
public:
  TRANSLATOR_DECLARATIONS (Key_performer);
  ~Key_performer ();

protected:
  void process_music ();
  void stop_translation_timestep ();

  DECLARE_TRANSLATOR_LISTENER (key_change);
private:
  Stream_event *key_ev_;
  Audio_key *audio_;
};

Key_performer::Key_performer ()
{
  key_ev_ = 0;
  audio_ = 0;
}

Key_performer::~Key_performer ()
{
}

void
Key_performer::process_music ()
{
  if (key_ev_)
    {
      SCM pitchlist = key_ev_->get_property ("pitch-alist");
      SCM proc = ly_lily_module_constant ("alterations-in-key");

      SCM acc = scm_call_1 (proc, pitchlist);

      Pitch key_do (0,
		    scm_to_int (scm_caar (pitchlist)),
		    ly_scm2rational (scm_cdar (pitchlist)));

      Pitch c_do (0, 0, 0);

      SCM c_pitchlist
	= ly_transpose_key_alist (pitchlist,
				  pitch_interval (key_do, c_do).smobbed_copy ());

      /* MIDI keys are too limited for lilypond scales.
	 We check for minor scale and assume major otherwise.  */

      SCM third = scm_assoc (scm_from_int (2),
			     c_pitchlist);
      bool minor = (scm_is_pair (third)
		    && scm_is_number (scm_cdr (third))
		    && ly_scm2rational (scm_cdr (third)) == FLAT_ALTERATION);

      audio_ = new Audio_key (scm_to_int (acc),
			      !minor);

      Audio_element_info info (audio_, key_ev_);
      announce_element (info);
      key_ev_ = 0;
    }
}

void
Key_performer::stop_translation_timestep ()
{
  if (audio_)
    {
      audio_ = 0;
    }
}

IMPLEMENT_TRANSLATOR_LISTENER (Key_performer, key_change);
void
Key_performer::listen_key_change (Stream_event *ev)
{
  if (!key_ev_)
    key_ev_ = ev;
}

ADD_TRANSLATOR (Key_performer,
		/* doc */
		"",

		/* create */
		"",

		/* read */
		"",

		/* write */
		""
		);
