/*
  pitch-interval.hh -- declare Pitch_interval

  source file of the GNU LilyPond music typesetter

  (c) 2004--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#ifndef PITCH_INTERVAL_HH
#define PITCH_INTERVAL_HH

#include "pitch.hh"

class Pitch_interval : public Drul_array<Pitch>
{
public:
  Pitch_interval ();
  Pitch_interval (Pitch, Pitch);
  void add_point (Pitch);
  bool is_empty () const;
};

#endif /* PITCH_INTERVAL_HH */
