/*
  audio-staff.cc -- implement Audio_staff

  source file of the GNU LilyPond music typesetter

  (c) 1997 Jan Nieuwenhuizen <jan@digicash.com>
 */

#include "audio-staff.hh"
#include "audio-item.hh"
#include "midi-item.hh"
#include "midi-stream.hh"
#include "midi-walker.hh"

void
Audio_staff::add (Audio_item* l)
{
  audio_item_l_list_.bottom().add (l);
}

void
Audio_staff::output (Midi_stream& midi_stream_r, int track_i)
{
  Midi_track midi_track;
  midi_track.number_i_ =  track_i;
  for ( Midi_walker i (this, &midi_track); i.ok(); i++ )
	i.process();
  midi_stream_r << midi_track;
}

IMPLEMENT_IS_TYPE_B1(Audio_staff, Audio_element);
