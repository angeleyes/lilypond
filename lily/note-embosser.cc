/*
  note-embosser.cc -- implement Note_embosser - capture note events

  source file of the GNU LilyPond music typesetter

  (c) 2007 Ralph Little
*/

#include "embosser.hh"
#include "global-context.hh"
#include "stream-event.hh"
#include "warn.hh"
#include "international.hh"
#include "translator.icc"

class Note_embosser : public Embosser
{
public:
  TRANSLATOR_DECLARATIONS (Note_embosser);

protected:
  void stop_translation_timestep ();
  void process_music ();

  DECLARE_TRANSLATOR_LISTENER (note);
private:
};

void
Note_embosser::process_music ()
{
  warning(_ ("Embosser::process_music"));
}

void
Note_embosser::stop_translation_timestep ()
{
  warning(_ ("Embosser::stop_translation_timestep"));
}

IMPLEMENT_TRANSLATOR_LISTENER (Note_embosser, note)
void
Note_embosser::listen_note (Stream_event *ev)
{
  warning(_ ("Embosser::listen_note"));
}

ADD_TRANSLATOR (Note_embosser, "", "",
		"", "");

Note_embosser::Note_embosser ()
{
  warning(_ ("Note_embosser::Note_embosser"));

}
