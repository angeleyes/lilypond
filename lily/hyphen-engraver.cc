/*
  hyphen-engraver.cc -- implement Hyphen_engraver

  source file of the GNU LilyPond music typesetter

  (c)  1999--2003 Glen Prideaux <glenprideaux@iname.com>
*/

#include "flower-proto.hh"
#include "event.hh"
#include "hyphen-spanner.hh"
#include "paper-column.hh"
#include "item.hh"
#include "engraver.hh"

/**
  Generate an centred hyphen.  Should make a Hyphen_spanner that
  typesets a nice centred hyphen of varying length depending on the
  gap between syllables.

  We remember the last Item that come across. When we get a
  event, we create the spanner, and attach the left point to the
  last lyrics, and the right point to any lyrics we receive by
  then.  */
class Hyphen_engraver : public Engraver
{
  Grob *last_lyric_;
  Grob *current_lyric_;
  Music* req_;
  Spanner* hyphen_;
public:
  TRANSLATOR_DECLARATIONS(Hyphen_engraver);

protected:
  virtual void acknowledge_grob (Grob_info);
  virtual void finalize ();
  virtual bool try_music (Music*);
  virtual void stop_translation_timestep ();
  virtual void start_translation_timestep ();
  virtual void process_acknowledged_grobs ();
private:

};



Hyphen_engraver::Hyphen_engraver ()
{
  current_lyric_ = 0;
  last_lyric_ = 0;
  hyphen_ = 0;
  req_ = 0;
}

void
Hyphen_engraver::acknowledge_grob (Grob_info i)
{
  // -> text-item
  if (i.grob_->internal_has_interface (ly_symbol2scm ("lyric-syllable-interface")))
    {
      current_lyric_ = i.grob_;
      if (hyphen_
	  && !hyphen_->get_bound (RIGHT))
	  {
	    hyphen_->set_bound (RIGHT, i.grob_);
	  }
    }
}


bool
Hyphen_engraver::try_music (Music* r)
{
  if (req_)
	return false;

      req_ = r;
      return true;
}

void
Hyphen_engraver::finalize ()
{
  if (hyphen_)
    {
      req_->origin ()->warning (_ ("unterminated hyphen"));
      hyphen_->set_bound (RIGHT, unsmob_grob (get_property ("currentCommandColumn")));
    }
}

void
Hyphen_engraver::process_acknowledged_grobs ()
{
  if (req_ &&! hyphen_)
    {
      if (!last_lyric_)
	{
	  req_->origin ()->warning (_ ("Nothing to connect hyphen to on the left.  Ignoring hyphen event."));
	  return;
	}
      
      hyphen_ = new Spanner (get_property ("LyricHyphen"));

      hyphen_->set_bound (LEFT, last_lyric_);
      announce_grob(hyphen_, req_->self_scm());
    }
}


void
Hyphen_engraver::stop_translation_timestep ()
{
  if (hyphen_)
    {
      typeset_grob (hyphen_);
      hyphen_ = 0;
    }

  if (current_lyric_)
    {
      last_lyric_ = current_lyric_;
      current_lyric_ =0;
    }
}

void
Hyphen_engraver::start_translation_timestep ()
{
  req_ = 0;
}


ENTER_DESCRIPTION(Hyphen_engraver,
/* descr */       "Create lyric hyphens",
/* creats*/       "LyricHyphen",
/* accepts */     "hyphen-event",
/* acks  */      "lyric-syllable-interface",
/* reads */       "",
/* write */       "");
