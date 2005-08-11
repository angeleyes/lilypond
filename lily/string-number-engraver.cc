/*
  string-number-engraver.cc -- implement String_number_engraver

  source file of the GNU LilyPond music typesetter

  (c) 2005 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "engraver.hh"

//  Junk String numbers.
class String_number_engraver : public Engraver
{
public:
  TRANSLATOR_DECLARATIONS (String_number_engraver);
protected:
  virtual bool try_music (Music *m);
};

bool
String_number_engraver::try_music (Music *)
{
  return true;
}

String_number_engraver::String_number_engraver ()
{
}

/*
  TODO: string numbers are printed right of the note circled. This
  engraver should provide this functionality.
*/

#include "translator.icc"

ADD_TRANSLATOR (String_number_engraver,
		/* doc */ "Swallow string-number-events - the purpose of this engraver is to"
		" process tab for normal notation. To provent warnings for unprocessed "
		" string-number-event to obscure real error messages, this engraver "
		" swallows them all.",

		/* create */ "",
		/* accept */ "string-number-event",
		/* read */ "",
		/* write */ "");
