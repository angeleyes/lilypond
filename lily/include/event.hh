/*
  event.hh -- declare Event baseclasses.

  source file of the GNU LilyPond music typesetter

  (c)  1997--2003 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#ifndef EVENT_HH
#define EVENT_HH


#include "string.hh"
#include "moment.hh"
#include "virtual-methods.hh"
#include "input.hh"
#include "music.hh"
#include "duration.hh"
#include "pitch.hh"

/** An atom of musical information.  This is an abstract class for any
  piece of music that does not contain other Music.
  

 */
class Event : public Music {
public:
  Event ();
  VIRTUAL_COPY_CONS (Music);
  virtual void compress (Moment);
  virtual void transpose (Pitch);
  virtual Moment get_length () const;
  virtual Pitch to_relative_octave (Pitch);
};


/**
    Handle key changes.
*/
class Key_change_ev  : public Event
{
public:
  SCM pitch_alist ();
  
protected:
  VIRTUAL_COPY_CONS (Music);
  void transpose (Pitch  d);
};

SCM transpose_key_alist (SCM,SCM);



#endif
