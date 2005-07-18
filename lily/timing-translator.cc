/*
  timing-translator.cc -- implement Timing_translator


  source file of the GNU LilyPond music typesetter

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "timing-translator.hh"

#include "warn.hh"
#include "translator-group.hh"
#include "global-context.hh"
#include "multi-measure-rest.hh"

void
Timing_translator::stop_translation_timestep ()
{
  Global_context *global = get_global_context ();

  /* allbars == ! skipbars */
  SCM sb = get_property ("skipBars");
  bool allbars = !to_boolean (sb);

  // urg: multi bar rests: should always process whole of first bar?
  SCM tim = get_property ("timing");
  bool timb = to_boolean (tim);
  if (timb && allbars)
    {
      Moment barleft = (measure_length () - measure_position (context ()));
      Moment now = now_mom ();

      if (barleft > Moment (0)
	  /*
	    Hmm. We insert the bar moment every time we process a
	    moment.  A waste of cpu?
	  */
	  && !now.grace_part_)
	global->add_moment_to_process (now + barleft);
    }
}

void
Timing_translator::initialize ()
{

  /*
    move this to engraver-init.ly?
  */
  context ()->add_alias (ly_symbol2scm ("Timing"));
  context ()->set_property ("timing", SCM_BOOL_T);
  context ()->set_property ("currentBarNumber", scm_int2num (1));

  context ()->set_property ("timeSignatureFraction",
			    scm_cons (scm_int2num (4), scm_int2num (4)));
  /*
    Do not init measurePosition; this should be done from global
    context.
  */
  context ()->set_property ("measureLength", Moment (Rational (1)).smobbed_copy ());
  context ()->set_property ("beatLength", Moment (Rational (1, 4)).smobbed_copy ());
}

Rational
Timing_translator::measure_length () const
{
  SCM l = get_property ("measureLength");
  if (unsmob_moment (l))
    return unsmob_moment (l)->main_part_;
  else
    return Rational (1);
}

Timing_translator::Timing_translator ()
{
}


void
Timing_translator::start_translation_timestep ()
{
  Global_context *global = get_global_context ();

  Moment now = global->now_mom ();
  Moment dt = now - global->previous_moment ();
  if (dt < Moment (0))
    {
      programming_error ("moving backwards in time");
      dt = 0;
    }
  else if (dt.main_part_.is_infinity ())
    {
      programming_error ("moving infinitely to future");
      dt = 0;
    }

  if (!dt.to_bool ())
    return;

  Moment measposp;

  SCM s = get_property ("measurePosition");
  if (unsmob_moment (s))
    {
      measposp = *unsmob_moment (s);
    }
  else
    {
      measposp = now;
      context ()->set_property ("measurePosition",
				measposp.smobbed_copy ());
    }

  measposp += dt;

  SCM barn = get_property ("currentBarNumber");
  int b = 0;
  if (scm_is_number (barn))
    {
      b = scm_to_int (barn);
    }

  SCM cad = get_property ("timing");
  bool c = to_boolean (cad);

  Rational len = measure_length ();
  while (c && measposp.main_part_ >= len)
    {
      measposp.main_part_ -= len;
      b++;
    }

  context ()->set_property ("currentBarNumber", scm_int2num (b));
  context ()->set_property ("measurePosition", measposp.smobbed_copy ());
}

#include "translator.icc"

ADD_TRANSLATOR (Timing_translator,
		"This engraver adds the alias "
		"@code{Timing} to its containing context."
		"Responsible for synchronizing timing information from staves.  "
		"Normally in @code{Score}.  In order to create polyrhythmic music, "
		"this engraver should be removed from @code{Score} and placed in "
		"@code{Staff}. "
		"\n\nThis engraver adds the alias @code{Timing} to its containing context."

		,

		"", "", "", "", "");
