/*
  hyphen-engraver.cc -- implement Hyphen_engraver

  source file of the GNU LilyPond music typesetter

  (c) 1999--2005 Glen Prideaux <glenprideaux@iname.com>,
  Han-Wen Nienhuys <hanwen@xs4all.nl>,
  Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include "engraver.hh"

#include "warn.hh"
#include "item.hh"
#include "spanner.hh"
#include "pointer-group-interface.hh"

class Hyphen_engraver : public Engraver
{
  Music *ev_;
  Music *finished_ev_;

  Spanner *hyphen_;
  Spanner *finished_hyphen_;

public:
  TRANSLATOR_DECLARATIONS (Hyphen_engraver);

protected:

  DECLARE_ACKNOWLEDGER (lyric_syllable);

  virtual void finalize ();
  virtual bool try_music (Music *);

  void stop_translation_timestep ();
  void process_music ();
};

Hyphen_engraver::Hyphen_engraver ()
{
  hyphen_ = 0;
  finished_hyphen_ = 0;
  finished_ev_ = 0;
  ev_ = 0;
}

void
Hyphen_engraver::acknowledge_lyric_syllable (Grob_info i)
{
  Item *item = i.item ();
  
  if (!hyphen_)
    hyphen_ = make_spanner ("LyricSpace", item->self_scm ());

  if (hyphen_)
    hyphen_->set_bound (LEFT, item);
      
  if (finished_hyphen_)
    finished_hyphen_->set_bound (RIGHT, item);
}

bool
Hyphen_engraver::try_music (Music *r)
{
  if (ev_)
    return false;

  ev_ = r;
  return true;
}

void
completize_hyphen (Spanner *sp)
{
  if (!sp->get_bound (RIGHT))
    {
      extract_item_set (sp, "heads", heads);
      if (heads.size ())
	sp->set_bound (RIGHT, heads.top ());
    }
}

void
Hyphen_engraver::finalize ()
{
  if (hyphen_)
    {
      completize_hyphen (hyphen_);

      if (!hyphen_->get_bound (RIGHT))
	{
	  hyphen_->warning (_ ("removing unterminated hyphen"));
	  hyphen_->suicide ();
	}

      hyphen_ = 0;
    }

  if (finished_hyphen_)
    {
      completize_hyphen (finished_hyphen_);

      if (!finished_hyphen_->get_bound (RIGHT))
	{
	  if (finished_ev_)
	    finished_hyphen_->warning (_ ("unterminated hyphen; removing"));
	  finished_hyphen_->suicide ();
	}
      finished_hyphen_ = 0;
    }
}

void
Hyphen_engraver::process_music ()
{
  if (ev_)
    hyphen_ = make_spanner ("LyricHyphen", ev_->self_scm ());
}

void
Hyphen_engraver::stop_translation_timestep ()
{
  if (finished_hyphen_ && finished_hyphen_->get_bound (RIGHT))
    {
      finished_hyphen_ = 0;
      finished_ev_ = 0;
    }
  
  if (finished_hyphen_ && hyphen_)
    {
      programming_error ("hyphen not finished yet");
      finished_hyphen_ = 0;
      finished_ev_ = 0;
    }

  if (hyphen_)
    {
      finished_hyphen_ = hyphen_;
      finished_ev_ = ev_;
    }
  
  hyphen_ = 0;
  ev_ = 0;
}

#include "translator.icc"

ADD_ACKNOWLEDGER (Hyphen_engraver, lyric_syllable);

ADD_TRANSLATOR (Hyphen_engraver,
		/* doc */ "Create lyric hyphens",
		/* create */ "LyricHyphen LyricsSpace",
		/* accept */ "hyphen-event",
		/* read */ "",
		/* write */ "");
