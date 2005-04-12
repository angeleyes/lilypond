/*
  time-signature-engraver.cc -- implement Time_signature_engraver

  source file of the GNU LilyPond music typesetter

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "time-signature.hh"
#include "warn.hh"
#include "engraver-group-engraver.hh"
#include "misc.hh"

/**
   generate time_signatures.
*/
class Time_signature_engraver : public Engraver
{
  Item *time_signature_;
  SCM last_time_fraction_;

protected:
  virtual void stop_translation_timestep ();
  virtual void process_music ();
public:
  TRANSLATOR_DECLARATIONS (Time_signature_engraver);
};

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

ADD_TRANSLATOR (Time_signature_engraver,
		/* descr */ "Create a TimeSignature whenever @code{timeSignatureFraction} changes",
		/* creats*/ "TimeSignature",
		/* accepts */ "",
		/* acks  */ "",
		/* reads */ "",
		/* write */ "");
