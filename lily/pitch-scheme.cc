/*
  pitch-scheme.cc -- implement scheme functions for Pitch

  source file of the GNU LilyPond music typesetter

  (c) 2005--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>
  2007 Rune Zedeler <rune@zedeler.dk>
*/

#include "pitch.hh"
#include <iostream>

LY_DEFINE (ly_pitch_transpose, "ly:pitch-transpose",
	   2, 0, 0, (SCM p, SCM delta),
	   "Transpose @var{p} by the amount @var{delta},"
	   " where @var{delta} is relative to middle@tie{}C.")
{
  LY_ASSERT_SMOB (Pitchclass, p, 1);
  LY_ASSERT_SMOB (Pitchclass, delta, 2);
  Pitchclass *t = unsmob_pitchclass (p);
  Pitchclass *d = unsmob_pitchclass (delta);
  return t->transposed (*d).smobbed_clone ();
}

/* Should add optional args.  */
LY_DEFINE (ly_make_pitch, "ly:make-pitch",
	   2, 1, 0, (SCM octave, SCM note, SCM alter),
	   "@var{octave} is specified by #f to indicate pitchclass"
	   " or an integer, zero for the octave"
	   " containing middle@tie{}C.  @var{note} is a number from 0"
	   " to@tie{}6, with 0 corresponding to pitch@tie{}C and 6"
	   " corresponding to pitch@tie{}B.  @var{alter} is a rational"
	   " number of whole tones for alteration.")
{
  LY_ASSERT_TYPE (scm_is_integer, octave, 1);
  LY_ASSERT_TYPE (scm_is_integer, note, 2);
  LY_ASSERT_TYPE (scm_is_rational, alter, 3);
  
  Pitch p (scm_to_int (octave), scm_to_int (note),
	   ly_scm2rational (alter));
  
  return p.smobbed_clone ();
}

LY_DEFINE (ly_pitch_negate, "ly:pitch-negate", 1, 0, 0,
	   (SCM pp),
	   "Negate @var{p}.")
{
  LY_ASSERT_TYPE (unsmob_pitch, pp, 1);
  Pitch *p = unsmob_pitch (pp);
  return p->negated ().smobbed_clone ();
}

LY_DEFINE (ly_pitch_steps, "ly:pitch-steps", 1, 0, 0,
	   (SCM pp),
	   "Number of steps counted from middle@tie{}C of the"
	   " pitch@tie{}@var{p}.")
{
  LY_ASSERT_TYPE (unsmob_pitch, pp, 1);
  Pitch *p = unsmob_pitch (pp);
  return scm_from_int (p->steps ());
}

LY_DEFINE (ly_pitch_octave, "ly:pitch-octave",
	   1, 0, 0, (SCM pp),
	   "Extract the octave from pitch@tie{}@var{pp}.")
{
  LY_ASSERT_SMOB (Pitchclass, pp, 1);  
  Pitchclass *pc = unsmob_pitchclass (pp);
  if (Pitch *p = dynamic_cast<Pitch*> (pc))
    return scm_from_int(p->get_octave());
  else
    return SCM_BOOL_F;
}

LY_DEFINE (ly_pitch_alteration, "ly:pitch-alteration",
	   1, 0, 0, (SCM pp),
	   "Extract the alteration from pitch@tie{}@var{pp}.")
{
  LY_ASSERT_SMOB (Pitchclass, pp, 1);  
  Pitchclass *pc = unsmob_pitchclass (pp);
  Rational q = pc->get_alteration ();

  return ly_rational2scm (q);
}

LY_DEFINE (pitch_notename, "ly:pitch-notename",
	   1, 0, 0, (SCM pp),
	   "Extract the note name from pitch @var{pp}.")
{
  LY_ASSERT_SMOB (Pitchclass, pp, 1);  
  Pitchclass *pc = unsmob_pitchclass (pp);
  int q = pc->get_notename ();

  return scm_from_int (q);
}

LY_DEFINE (ly_pitch_quartertones, "ly:pitch-quartertones",
	   1, 0, 0, (SCM pp),
	   "Calculate the number of quarter tones of@tie{}@var{pp} from"
	   " middle@tie{}C.")
{
  LY_ASSERT_TYPE (unsmob_pitch, pp, 1);  
  Pitch *p = unsmob_pitch (pp);

  int q = p->rounded_quartertone_pitch ();
  return scm_from_int (q);
}

LY_DEFINE (ly_pitch_semitones, "ly:pitch-semitones",
	   1, 0, 0, (SCM pp),
	   "Calculate the number of semitones of@tie{}@var{pp} from"
	   " middle@tie{}C.")
{
  LY_ASSERT_TYPE (unsmob_pitch, pp, 1);  
  Pitch *p = unsmob_pitch (pp);

  int q = p->rounded_semitone_pitch ();
  return scm_from_int (q);
}

LY_DEFINE (ly_pitch_less_p, "ly:pitch<?",
	   2, 0, 0, (SCM p1, SCM p2),
	   "Is @var{p1} lexicographically smaller than @var{p2}?")
{
  LY_ASSERT_SMOB (Pitchclass, p1, 1);
  LY_ASSERT_SMOB (Pitchclass, p2, 2);

  Pitchclass *a = unsmob_pitchclass (p1);
  Pitchclass *b = unsmob_pitchclass (p2);

  return (Pitchclass::compare (*a, *b) < 0) ? SCM_BOOL_T : SCM_BOOL_F;
}

LY_DEFINE (ly_pitch_diff, "ly:pitch-diff",
	   2, 0, 0, (SCM pitch, SCM root),
	   "Return pitch @var{delta} such that @var{pitch} transposed by"
	   " @var{delta} equals @var{root}.")
{
  LY_ASSERT_TYPE (unsmob_pitch, pitch, 1);  
  LY_ASSERT_TYPE (unsmob_pitch, root, 2);  
  Pitch *p = unsmob_pitch (pitch);
  Pitch *r = unsmob_pitch (root);

  return pitch_interval (*r, *p).smobbed_clone ();
}
