/*   
  simultaneous-music.cc --  implement Simultaneous_music

  source file of the GNU LilyPond music typesetter

  (c) 1998--2003 Han-Wen Nienhuys <hanwen@xs4all.nl>

*/

#include "input.hh"
#include "moment.hh"
#include "pitch.hh"
#include "music-list.hh"
#include "scm-option.hh"

Moment
Simultaneous_music::get_length () const
{
  return Music_sequence::maximum_length (get_mus_property ("elements"));
}

Moment
Simultaneous_music::start_mom () const
{
  return Music_sequence::minimum_start (get_mus_property ("elements"));
}

Simultaneous_music::Simultaneous_music()
{

}

Pitch
Simultaneous_music::to_relative_octave (Pitch p)
{
  if (lily_1_8_relative)
    lily_1_8_compatibility_used = true;
  
  return music_list_to_relative (music_list(), p, lily_1_8_relative);
}

ADD_MUSIC (Simultaneous_music);

Pitch
Event_chord::to_relative_octave (Pitch p)
{
  return music_list_to_relative (music_list (), p, true);
}

ADD_MUSIC(Event_chord);
