/*   
  transposed-music.hh -- declare Transposed_music
  
  source file of the GNU LilyPond music typesetter
  
  (c) 1998--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  
 */

#ifndef TRANSPOSED_MUSIC_HH
#define TRANSPOSED_MUSIC_HH

#include "music-wrapper.hh"

class Transposed_music : public Music_wrapper
{
public:
  Transposed_music ();
  Transposed_music (SCM);
  VIRTUAL_COPY_CONSTRUCTOR (Music, Transposed_music);

  virtual Pitch to_relative_octave (Pitch);
};

#endif /* TRANSPOSED_MUSIC_HH */

