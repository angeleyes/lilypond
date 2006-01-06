/*
  performer.cc -- implement Performer

  source file of the GNU LilyPond music typesetter

  (c) 1996--2006 Han-Wen Nienhuys <hanwen@xs4all.nl>
  Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include "context.hh"
#include "performer-group.hh"
#include "warn.hh"

void
Performer::play_element (Audio_element *p)
{
  get_daddy_performer ()->play_element (p);
}

int
Performer::get_tempo () const
{
  return get_daddy_performer ()->get_tempo ();
}

Performer_group *
Performer::get_daddy_performer () const
{
  return
    dynamic_cast<Performer_group *> (get_daddy_translator ());
}

void
Performer::acknowledge_audio_element (Audio_element_info)
{
}

void
Performer::create_audio_elements ()
{
}

void
Performer::announce_element (Audio_element_info i)
{
  if (!i.origin_trans_)
    i.origin_trans_ = this;

  get_daddy_performer ()->announce_element (i);
}
