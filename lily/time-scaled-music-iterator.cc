/*   
  time-scaled-music-iterator.cc -- implement Time_scaled_music_iterator
  
  source file of the GNU LilyPond music typesetter
  
  (c) 1998--2003 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  
 */

#include "time-scaled-music-iterator.hh"
#include "time-scaled-music.hh"
#include "event.hh"
#include "translator-group.hh"


void
Time_scaled_music_iterator::process (Moment m)
{
  if (!m.to_bool ())
    {
      Music_iterator *yeah = try_music (get_music ());
      if (yeah)
	set_translator (yeah->report_to ());
      else
	get_music ()->origin ()->warning (_ ("no one to print a tuplet start bracket"));
    }

  Music_wrapper_iterator::process (m);
}
 
IMPLEMENT_CTOR_CALLBACK (Time_scaled_music_iterator);
