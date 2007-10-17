#ifndef MUSICAL_PITCHCLASS_HH
#define MUSICAL_PITCHCLASS_HH

#include "lily-proto.hh"
#include "smobs.hh"
#include "rational.hh"


class Pitchclass
{
protected:
  int notename_;
  Rational alteration_;
  Scale *scale_;

  void transpose (Pitchclass);
  void normalize ();

public:
  int get_notename () const;
  Rational get_alteration () const;

  Pitchclass (int notename, Rational accidental);
  Pitchclass (int notename);
  Pitchclass ();

  virtual ~Pitchclass () {}

  Pitchclass transposed (Pitchclass) const;

  static int compare (Pitchclass const &, Pitchclass const &);

  virtual Rational tone_pitch () const;
  int rounded_semitone_pitch () const;
  int rounded_quartertone_pitch () const;

  Pitchclass negated () const;
  string to_string () const;

  DECLARE_SCHEME_CALLBACK (less_p, (SCM a, SCM b));
  DECLARE_SIMPLE_SMOBS (Pitchclass);
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

#endif /* MUSICAL_PITCHCLASS_HH */
