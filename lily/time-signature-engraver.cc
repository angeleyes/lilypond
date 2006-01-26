/*
  time-signature-engraver.cc -- implement Time_signature_engraver

  source file of the GNU LilyPond music typesetter

  (c) 1997--2006 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "engraver-group.hh"

#include "international.hh"
#include "misc.hh"
#include "time-signature.hh"
#include "warn.hh"

/**
   generate time_signatures.
*/
class Time_signature_engraver : public Engraver
{
  Item *time_signature_;
  SCM last_time_fraction_;

protected:
  virtual void derived_mark () const;
  void stop_translation_timestep ();
  void process_music ();
public:
  TRANSLATOR_DECLARATIONS (Time_signature_engraver);
};

void
Time_signature_engraver::derived_mark () const
{
  scm_gc_mark (last_time_fraction_);
}

Time_signature_engraver::Time_signature_engraver ()
{
  time_signature_ = 0;
  last_time_fraction_ = SCM_BOOL_F;
}

void
Time_signature_engraver::process_music ()
{
  /*
    not rigorously safe, since the value might get GC'd and
    reallocated in the same spot */
  SCM fr = get_property ("timeSignatureFraction");
  if (!time_signature_
      && last_time_fraction_ != fr
      && scm_is_pair (fr))
    {
      int den = scm_to_int (scm_cdr (fr));
      if (den != (1 << intlog2 (den)))
	{
	  /*
	    Todo: should make typecheck?

	    OTOH, Tristan Keuris writes 8/20 in his Intermezzi.
	  */
	  warning (_f ("strange time signature found: %d/%d",
		       den,
		       scm_to_int (scm_car (fr))));
	}

      last_time_fraction_ = fr;
      time_signature_ = make_item ("TimeSignature", SCM_EOL);
      time_signature_->set_property ("fraction", fr);
    }
}

void
Time_signature_engraver::stop_translation_timestep ()
{
  time_signature_ = 0;
}

#include "translator.icc"

ADD_TRANSLATOR (Time_signature_engraver,
		/* doc */ "Create a TimeSignature whenever @code{timeSignatureFraction} changes",
		/* create */ "TimeSignature",
		/* accept */ "",
		/* read */ "",
		/* write */ "");
