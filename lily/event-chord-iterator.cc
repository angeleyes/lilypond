/*
  event-chord-iterator.cc -- implement Event_chord_iterator

  source file of the GNU LilyPond music typesetter

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "event-chord-iterator.hh"

#include "context.hh"
#include "warn.hh"
#include "event.hh"

Event_chord_iterator::Event_chord_iterator ()
{
}

Context *
Event_chord_iterator::get_req_translator ()
{
  assert (get_outlet ());
  if (get_outlet ()->is_bottom_context ())
    return get_outlet ();

  set_context (get_outlet ()->get_default_interpreter ());
  return get_outlet ();
}

void
Event_chord_iterator::construct_children ()
{
  Simple_music_iterator::construct_children ();
  get_req_translator ();
}

Event_chord *
Event_chord_iterator::get_elt () const
{
  return (Event_chord *) get_music ();
}

void
Event_chord_iterator::process (Moment m)
{
  if (last_processed_mom_ < Moment (0))
    {
      for (SCM s = get_music ()->get_property ("elements");
	   scm_is_pair (s); s = scm_cdr (s))
	{
	  Music *mus = unsmob_music (scm_car (s));

	  bool gotcha = try_music (mus);
	  if (!gotcha)
	    mus->origin ()->warning (_f ("Junking event: `%s'", mus->name ()));
	}
    }
  Simple_music_iterator::process (m);
}

IMPLEMENT_CTOR_CALLBACK (Event_chord_iterator);
