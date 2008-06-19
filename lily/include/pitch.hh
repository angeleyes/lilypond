/*
  pitch.hh -- declare Pitch

  source file of the GNU LilyPond music typesetter

  (c) 1998--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>
  2007--2008 Rune Zedeler
*/

#ifndef PITCH_HH
#define PITCH_HH

#include "lily-proto.hh"
#include "smobs.hh"
#include "rational.hh"
#include "pitchclass.hh"


/** A "tonal" pitch. This is a pitch used in diatonal western music
    (24 quartertones in an octave), as opposed to a frequency in Hz or a
    integer number of semitones.

    Pitch is lexicographically ordered by (octave, notename,
    alteration).

*/
class Pitch : public Pitchclass
{
private:
  int octave_;

  void up_to (int);
  void down_to (int);
  void normalize ();

  void transpose (Pitch);

  string get_class_name () const;

public:
  Pitchclass * clone() const;
  int get_octave () const;

  Pitch (Pitch const *p);
  Pitch (int octave, int notename, Rational accidental);
  Pitch (int octave, int notename);
  Pitch ();

  Pitch transposed (Pitch) const;
  Pitchclass transposed (Pitchclass) const;

  Pitch to_relative_octave (Pitch) const;

  int compare_to (Pitchclass const &) const;

  int steps () const;
  Rational tone_pitch () const;

  Pitch negated () const;
  string to_string () const;

  SCM smobbed_copy () const { return smobbed_clone (); }
};

SCM ly_pitch_diff (SCM pitch, SCM root);
SCM ly_pitch_transpose (SCM p, SCM delta);

Pitch * unsmob_pitch (SCM s);

Pitch pitch_interval (Pitch const &from, Pitch const &to);

Pitch * unsmob_pitch (SCM s);

#endif /* PITCH_HH */

