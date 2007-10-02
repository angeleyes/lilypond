/*
  score-embosser.cc -- implement Score_embosser - pulls embossing output all together.

  source file of the GNU LilyPond music typesetter

  (c) 2007 Ralph Little
*/

#include "score-embosser.hh"

#include "context-def.hh"
#include "dispatcher.hh"
#include "global-context.hh"
#include "embosser-output.hh"
#include "output-def.hh"
#include "string-convert.hh"
#include "warn.hh"
#include "international.hh"

ADD_TRANSLATOR_GROUP (Score_embosser,
		      /* doc */ "",
		      /* create */ "",
		      /* read */ "",
		      /* write */ "");

Score_embosser::Score_embosser ()
{
}

Score_embosser::~Score_embosser ()
{
}

void
Score_embosser::connect_to_context (Context *c)
{
  Embosser_group::connect_to_context (c);

  Dispatcher *d = c->get_global_context ()->event_source ();
  d->add_listener (GET_LISTENER (one_time_step), ly_symbol2scm ("OneTimeStep"));
  d->add_listener (GET_LISTENER (prepare), ly_symbol2scm ("Prepare"));
  d->add_listener (GET_LISTENER (finish), ly_symbol2scm ("Finish"));
}

void
Score_embosser::disconnect_from_context ()
{
  Dispatcher *d = context ()->get_global_context ()->event_source ();
  d->remove_listener (GET_LISTENER (one_time_step), ly_symbol2scm ("OneTimeStep"));
  d->remove_listener (GET_LISTENER (prepare), ly_symbol2scm ("Prepare"));
  d->remove_listener (GET_LISTENER (finish), ly_symbol2scm ("Finish"));

  Embosser_group::disconnect_from_context ();
}

IMPLEMENT_LISTENER (Score_embosser, prepare);
void
Score_embosser::prepare (SCM sev)
{
  precomputed_recurse_over_translators (context (), START_TRANSLATION_TIMESTEP, UP);
}

IMPLEMENT_LISTENER (Score_embosser, finish);
void
Score_embosser::finish (SCM)
{
  recurse_over_translators (context (),
			    &Translator::finalize,
			    &Translator_group::finalize,
			    UP);
}

IMPLEMENT_LISTENER (Score_embosser, one_time_step);
void
Score_embosser::one_time_step (SCM)
{
  if (!to_boolean (context ()->get_property ("skipTypesetting")))
    {
      precomputed_recurse_over_translators (context (), PROCESS_MUSIC, UP);
      do_announces ();
    }

  precomputed_recurse_over_translators (context (), STOP_TRANSLATION_TIMESTEP, UP);
}

void
Score_embosser::derived_mark () const
{
	Embosser_group::derived_mark ();
}

void
Score_embosser::initialize ()
{
	Translator_group::initialize ();
}


