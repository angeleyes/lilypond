/*
  music.hh -- declare Music

  source file of the GNU LilyPond music typesetter

  (c)  1997--2000 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/



#ifndef MUSIC_HH
#define MUSIC_HH

#include "virtual-methods.hh"
#include "input.hh"
#include "minterval.hh"
#include "lily-proto.hh"
#include "string.hh"

/** Music is anything that has duration and supports both time compression and
  transposition.
  
  In Lily, everything that can be thought to have a length and a pitch
  (which has a duration which can be transposed) is considered "music",

  Music is hierarchical: 

  @see Music_sequence

  */
class Music:public Input {
public:
    

  virtual Musical_pitch to_relative_octave (Musical_pitch);

  /// The duration of this piece of music
  virtual Moment length_mom () const;

  virtual ~Music(){}
  void print() const;
  /// Transpose, with the interval central C to #p#
  virtual void transpose (Musical_pitch p);

  /// Scale the music in time by #factor#.
  virtual void compress (Moment factor);
  VIRTUAL_COPY_CONS(Music);
  
  Music();
protected:
  virtual void do_print() const;
};

#endif // MUSIC_HH



