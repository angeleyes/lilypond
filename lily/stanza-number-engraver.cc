/*   
  lyric-number-engraver.cc --  implement Stanza_number_engraver
  
  source file of the GNU LilyPond music typesetter
  
  (c) 2000--2001 Han-Wen Nienhuys <hanwen@cs.uu.nl>, Glen Prideaux <glenprideaux@iname.com>
  
  Similar to (and derived from) Instrument_name_engraver.
 */

#include "engraver.hh"
#include "item.hh"
//#include "system-start-delimiter.hh"
//#include "side-position-interface.hh"
//#include "staff-symbol-referencer.hh"
#include "bar.hh"

class Stanza_number_engraver : public Engraver
{
  Item *text_;
  bool bar_b_;;

  void create_text (SCM s);
public:
  VIRTUAL_COPY_CONS (Translator);
  Stanza_number_engraver ();

  virtual void acknowledge_grob (Grob_info);
  virtual void stop_translation_timestep ();
};

ADD_THIS_TRANSLATOR (Stanza_number_engraver);

Stanza_number_engraver::Stanza_number_engraver ()
{
  text_ = 0;
  bar_b_ = false;
}

void
Stanza_number_engraver::acknowledge_grob (Grob_info i)
{
  if (gh_string_p (get_property ("whichBar")))
    {
      SCM s = get_property ("stanza");
      
      if (now_mom () > Moment (0))
	s = get_property ("stz");


      // TODO
      if (gh_string_p (s) || gh_pair_p (s))
	

	/*
	  if (i.elem_l_->has_interface (symbol ("lyric-syllable-interface")))

	  Tried catching lyric items to generate stanza numbers, but it
	  spoils lyric spacing.

	  Works, but requires bar_engraver in LyricsVoice context apart
	  from at beginning.  Is there any score element we can catch
	  that will do the trick?

	  What happens if we try anything at all EXCEPT a lyric? Is
	  there anything else?  Not sure what it's catching, but it
	  still mucks up lyrics.

	*/

	create_text (s);
    }
}

void
Stanza_number_engraver::stop_translation_timestep ()
{
  if (text_)
    {
      typeset_grob (text_);
      text_ = 0;
    }
}

void
Stanza_number_engraver::create_text (SCM txt)
{
  if (!text_)
    {
      text_ = new Item (get_property ("StanzaNumber"));
      text_->set_grob_property ("text", txt);
      announce_grob (text_,0);
    }
}




