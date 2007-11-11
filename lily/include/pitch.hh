/*
  pitch.hh -- declare Pitch

  source file of the GNU LilyPond music typesetter

  (c) 1998--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#ifndef MUSICAL_PITCH_HH
#define MUSICAL_PITCH_HH

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

public:
  int get_octave () const;

  Pitch (int octave, int notename, Rational accidental);
  Pitch (int octave, int notename);
  Pitch ();

  Pitch transposed (Pitch) const;
  Pitchclass transposed (Pitchclass) const;

  Pitch to_relative_octave (Pitch) const;

  static int compare (Pitch const &, Pitch const &);

  int steps () const;
  Rational tone_pitch () const;
  //int rounded_semitone_pitch () const;
  //int rounded_quartertone_pitch () const;

  Pitch negated () const;
  string to_string () const;
  SCM virtual_smobbed_copy () { // AAARGH!
    return smobbed_copy ();
  }

  DECLARE_SCHEME_CALLBACK (less_p, (SCM a, SCM b));
  DECLARE_SIMPLE_SMOBS (Pitch);
};

SCM ly_pitch_diff (SCM pitch, SCM root);
SCM ly_pitch_transpose (SCM p, SCM delta);
DECLARE_UNSMOB (Pitch, pitch);

Pitchclass *
unsmob_pitch_or_pitchclass (SCM s, int number);

INSTANTIATE_COMPARE (Pitch, Pitch::compare);

extern SCM pitch_less_proc;
Pitch pitch_interval (Pitch const &from, Pitch const &to);

#endif /* MUSICAL_PITCH_HH */

