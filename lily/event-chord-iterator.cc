/*
  event-chord-iterator.cc -- implement Event_chord_iterator

  source file of the GNU LilyPond music typesetter

  (c)  1997--2003 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "translator-group.hh"
#include "warn.hh"
#include "event-chord-iterator.hh"
#include "music-list.hh"
#include "event.hh"

Event_chord_iterator::Event_chord_iterator ()
{
}

Event_chord_iterator::Event_chord_iterator (Event_chord_iterator const &src)
  : Simple_music_iterator (src)
{
}

Translator_group*
Event_chord_iterator::get_req_translator ()
{
  assert (report_to ());
  if (report_to ()->is_bottom_translator_b ())
    return report_to ();

  set_translator (report_to ()->get_default_interpreter ());
  return report_to ();
}

void
Event_chord_iterator::construct_children ()
{
  Simple_music_iterator::construct_children ();
  get_req_translator ();
}

Event_chord*
Event_chord_iterator::get_elt () const
{
  return (Event_chord*) get_music ();
}

SCM
Event_chord_iterator::get_pending_events (Moment) const
{
  SCM s = SCM_EOL;
  if (last_processed_mom_ < Moment (0))
    {
      Music_sequence * ms = dynamic_cast<Music_sequence*> (get_music ());
     
      for (SCM m = ms->music_list (); gh_pair_p (m); m = ly_cdr (m))
	{
	  s = gh_cons (ly_car (m) , s);
	}
    }
  return s;
}

void
Event_chord_iterator::process (Moment m)
{
  if (last_processed_mom_ < Moment (0))
    {
      for (SCM s = dynamic_cast<Music_sequence *> (get_music ())->music_list ();
	   gh_pair_p (s);  s = ly_cdr (s))
	{
	  Music *mus = unsmob_music (ly_car (s));

	  bool gotcha = try_music (mus);
	  if (!gotcha)
	    mus->origin ()->warning (_f ("Junking event: `%s'", mus->name()));
	}
    }
  skip (m);
}

IMPLEMENT_CTOR_CALLBACK (Event_chord_iterator);
