/*   
  music-constructor.cc --  implement Music_constructor
  
  source file of the GNU LilyPond music typesetter
  
  (c) 2001--2004  Han-Wen Nienhuys <hanwen@cs.uu.nl>
  
 */

#include "music-constructor.hh"

#include <map>			// UGH.
#include <cassert>

#include "warn.hh"

static std::map<String,Music_ctor> *ctors_map_;

void
add_music_ctor (String s, Music_ctor c)
{
  if (!ctors_map_)
    ctors_map_ = new std::map<String, Music_ctor>;
  
 (*ctors_map_)[s] = c;
}

Music_ctor
get_music_ctor (String s)
{
  if (ctors_map_->find (s) == ctors_map_->end ())
    return 0;

  return (*ctors_map_)[s];
}

Music * 
make_music (String s, SCM init)
{
  Music_ctor c = get_music_ctor (s);
  if (!c)
    programming_error (String ("No constructor for music: ") + s);
  assert (c);
  
  return (*c) (init);
}

