/*
  pitchclass.hh -- declare Pitchclass

  source file of the GNU LilyPond music typesetter

  2007--2008 Rune Zedeler <rz@daimi.au.dk>
*/

#ifndef PITCHCLASS_HH
#define PITCHCLASS_HH

#include "lily-proto.hh"
#include "smobs.hh"
#include "rational.hh"
#include <string>


/** A "tonal" class of pitches with the same tonality.
    Pitchclass is subclassed by Pitch, that adds octave information thereby
    distingguishing pitches of the same pitchclass.
    
    Pitchclass is lexicographically ordered by fifths (Bb<F<C<G, etc),
    Pitchclasses always coming before Pitches.

*/

class Pitchclass
{
protected:
  int notename_;
  Rational alteration_;
  Scale *scale_;

  void transpose (Pitchclass);
  void normalize ();

  virtual string get_class_name () const;

public:
  virtual Pitchclass * clone() const;
  int get_notename () const;
  Rational get_alteration () const;

  Pitchclass (Pitchclass const *pc);
  Pitchclass (int notename, Rational accidental);
  Pitchclass (int notename);
  Pitchclass ();

  virtual ~Pitchclass () {}

  virtual Pitchclass transposed (Pitchclass) const;

  static int compare (Pitchclass const &, Pitchclass const &);
  virtual int compare_to (Pitchclass const &) const;

  virtual Rational tone_pitch () const;
  int rounded_semitone_pitch () const;
  int rounded_quartertone_pitch () const;

  Pitchclass negated () const;
  virtual string to_string () const;

  DECLARE_SCHEME_CALLBACK (less_p, (SCM a, SCM b));
  DECLARE_VIRTUAL_SMOBS (Pitchclass);
};


enum {
  DOUBLE_FLAT = -4,
  THREE_Q_FLAT,
  FLAT,
  SEMI_FLAT,
  NATURAL,
  SEMI_SHARP,
  SHARP,
  THREE_Q_SHARP,
  DOUBLE_SHARP,
};

extern Rational  DOUBLE_FLAT_ALTERATION;
extern Rational  THREE_Q_FLAT_ALTERATION;
extern Rational  FLAT_ALTERATION;
extern Rational  SEMI_FLAT_ALTERATION;
extern Rational  NATURAL_ALTERATION;
extern Rational  SEMI_SHARP_ALTERATION;
extern Rational  SHARP_ALTERATION;
extern Rational  THREE_Q_SHARP_ALTERATION;
extern Rational  DOUBLE_SHARP_ALTERATION;

//SCM ly_pitch_diff (SCM pitch, SCM root);
//SCM ly_pitch_transpose (SCM p, SCM delta);
DECLARE_UNSMOB (Pitchclass, pitchclass);

INSTANTIATE_COMPARE (Pitchclass, Pitchclass::compare);

extern SCM pitchclass_less_proc;
Pitchclass pitchclass_interval (Pitchclass const &from, Pitchclass const &to);

#endif /* PITCHCLASS_HH */
