/*
  pitch-scheme.cc -- implement scheme functions for Pitch

  source file of the GNU LilyPond music typesetter

  (c) 2005--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>
  2007 Rune Zedeler <rune@zedeler.dk>
*/

#include "pitch.hh"

LY_DEFINE (ly_pitch_transpose, "ly:pitch-transpose",
	   2, 0, 0, (SCM p, SCM delta),
	   "Transpose @var{p} by the amount @var{delta},"
	   " where @var{delta} is relative to middle@tie{}C.")
{
  Pitchclass *t = unsmob_pitch_or_pitchclass (p,1);
  Pitchclass *d = unsmob_pitch_or_pitchclass (delta,2);
  return t->transposed (*d).smobbed_copy ();
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
  
  return p.smobbed_copy ();
}

LY_DEFINE (ly_pitch_negate, "ly:pitch-negate", 1, 0, 0,
	   (SCM p),
	   "Negate @var{p}.")
{
  LY_ASSERT_SMOB (Pitch, p, 1);
  Pitch *pp = unsmob_pitch (p);
  return pp->negated ().smobbed_copy ();
}

LY_DEFINE (ly_pitch_steps, "ly:pitch-steps", 1, 0, 0,
	   (SCM p),
	   "Number of steps counted from middle@tie{}C of the"
	   " pitch@tie{}@var{p}.")
{
  LY_ASSERT_SMOB (Pitch, p, 1);
  Pitch *pp = unsmob_pitch (p);
  return scm_from_int (pp->steps ());
}

LY_DEFINE (ly_pitch_octave, "ly:pitch-octave",
	   1, 0, 0, (SCM pp),
	   "Extract the octave from pitch@tie{}@var{pp}.")
{
  Pitch *p = unsmob_pitch (pp);
  if(p==NULL)
    {
      LY_ASSERT_SMOB (Pitchclass, pp, 1);
      return SCM_BOOL_F;
    }
  int q = p->get_octave ();
  return scm_from_int (q);
}

LY_DEFINE (ly_pitch_alteration, "ly:pitch-alteration",
	   1, 0, 0, (SCM pp),
	   "Extract the alteration from pitch@tie{}@var{pp}.")
{
  Pitchclass *pc = unsmob_pitch_or_pitchclass (pp, 1);
  Rational q = pc->get_alteration ();

  return ly_rational2scm (q);
}

LY_DEFINE (pitch_notename, "ly:pitch-notename",
	   1, 0, 0, (SCM pp),
	   "Extract the note name from pitch @var{pp}.")
{
  Pitchclass *p = unsmob_pitch_or_pitchclass (pp, 1);
  int q = p->get_notename ();
  return scm_from_int (q);
}

LY_DEFINE (ly_pitch_quartertones, "ly:pitch-quartertones",
	   1, 0, 0, (SCM pp),
	   "Calculate the number of quarter tones of@tie{}@var{pp} from"
	   " middle@tie{}C.")
{
  LY_ASSERT_SMOB (Pitch, pp, 1);
  Pitch *p = unsmob_pitch (pp);
  int q = p->rounded_quartertone_pitch ();
  return scm_from_int (q);
}

LY_DEFINE (ly_pitch_semitones, "ly:pitch-semitones",
	   1, 0, 0, (SCM pp),
	   "Calculate the number of semitones of@tie{}@var{pp} from"
	   " middle@tie{}C.")
{
  LY_ASSERT_SMOB (Pitch, pp, 1);
  Pitch *p = unsmob_pitch (pp);
  int q = p->rounded_semitone_pitch ();
  return scm_from_int (q);
}

LY_DEFINE (ly_pitch_less_p, "ly:pitch<?",
	   2, 0, 0, (SCM p1, SCM p2),
	   "Is @var{p1} lexicographically smaller than @var{p2}?")
{
  LY_ASSERT_SMOB (Pitch, p1, 1);
  LY_ASSERT_SMOB (Pitch, p2, 2);

  Pitch *a = unsmob_pitch (p1);
  Pitch *b = unsmob_pitch (p2);

  if (Pitch::compare (*a, *b) < 0)
    return SCM_BOOL_T;
  else
    return SCM_BOOL_F;
}

LY_DEFINE (ly_pitch_diff, "ly:pitch-diff",
	   2, 0, 0, (SCM pitch, SCM root),
	   "Return pitch @var{delta} such that @var{pitch} transposed by"
	   " @var{delta} equals @var{root}.")
{
 
  LY_ASSERT_SMOB (Pitch, pitch, 1);
  LY_ASSERT_SMOB (Pitch, root, 2);

  Pitch *p = unsmob_pitch (pitch);
  Pitch *r = unsmob_pitch (root);

  return pitch_interval (*r, *p).smobbed_copy ();
}
