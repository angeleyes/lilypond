/*   
  music-wrapper.hh -- declare Music_wrapper
  
  source file of the GNU LilyPond music typesetter
  
  (c)  1998--2003 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  
 */

#ifndef MUSIC_WRAPPER_HH
#define MUSIC_WRAPPER_HH

#include "music.hh"
#include "pitch.hh"

/** A Music that modifies an existing Music.  This data structure
  corresponds to a production that takes a single Music argument,
  
  Music: STUFF Music 

  */
class Music_wrapper : public Music
{
public:
  Music_wrapper ();
  Music * element () const;
  
  VIRTUAL_COPY_CONS (Music);
  virtual Moment get_length () const;
  virtual Moment start_mom () const;
};




#endif /* MUSIC_WRAPPER_HH */

