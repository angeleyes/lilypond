/*
  musical-pitch.cc -- implement Pitch

  source file of the GNU LilyPond music typesetter

  (c) 1998--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "pitch.hh"
#include "warn.hh"
#include "main.hh"

#include "ly-smobs.icc"

Pitch::Pitch (int o, int n, int a)
{
  notename_ = n;
  alteration_ = a;
  octave_ = o;
  normalise ();
}

/* FIXME: why is octave == 0 and default not middleC ? */
Pitch::Pitch ()
{
  notename_ = 0;
  alteration_ = 0;
  octave_ = 0;
}

int
Pitch::compare (Pitch const &m1, Pitch const &m2)
{
  int o = m1.octave_ - m2.octave_;
  int n = m1.notename_ - m2.notename_;
  int a = m1.alteration_ - m2.alteration_;

  if (o)
    return o;
  if (n)
    return n;
  if (a)
    return a;
  return 0;
}

int
Pitch::steps () const
{
  return notename_ + octave_ * 7;
}

/* Should be settable from input?  */
static Byte diatonic_scale_semitones[ ] = { 0, 2, 4, 5, 7, 9, 11 };

/* Calculate pitch height in 12th octave steps.  Don't assume
   normalised pitch as this function is used to normalise the pitch.  */
int
Pitch::semitone_pitch () const
{
  int o = octave_;
  int n = notename_;
  while (n < 0)
    {
      n += 7;
      o--;
    }

  if (alteration_ % 2)
    programming_error ("semitone_pitch () called on quarter tone alteration.");

  return ((o + n / 7) * 12
	  + diatonic_scale_semitones[n % 7]
	  + (alteration_ / 2));
}

int
Pitch::quartertone_pitch () const
{
  int o = octave_;
  int n = notename_;
  while (n < 0)
    {
      n += 7;
      o--;
    }

  return ((o + n / 7) * 24
	  + 2 * diatonic_scale_semitones[n % 7]
	  + alteration_);
}

void
Pitch::normalise ()
{
  int pitch = quartertone_pitch ();
  while (notename_ >= 7)
    {
      notename_ -= 7;
      octave_++;
      alteration_ -= quartertone_pitch () - pitch;
    }
  while (notename_ < 0)
    {
      notename_ += 7;
      octave_--;
      alteration_ -= quartertone_pitch () - pitch;
    }
  while (alteration_ > DOUBLE_SHARP)
    {
      if (notename_ == 6)
	{
	  notename_ = 0;
	  octave_++;
	}
      else
	notename_++;

      alteration_ = 0;
      alteration_ -= quartertone_pitch () - pitch;
    }

  while (alteration_ < DOUBLE_FLAT)
    {
      if (notename_ == 0)
	{
	  notename_ = 6;
	  octave_--;
	}
      else
	notename_--;

      alteration_ = 0;
      alteration_ -= quartertone_pitch () - pitch;
    }
}

/* WHugh, wat een intervaas */
void
Pitch::transpose (Pitch delta)
{
  int new_semi = quartertone_pitch () +delta.quartertone_pitch ();
  octave_ += delta.octave_;
  notename_ += delta.notename_;
  alteration_ += new_semi - quartertone_pitch ();

  normalise ();
}

Pitch
pitch_interval (Pitch const &from, Pitch const &to)
{
  int sound = to.quartertone_pitch () - from.quartertone_pitch ();
  Pitch pt (to.get_octave () - from.get_octave (),
	    to.get_notename () - from.get_notename (),

	    to.get_alteration () - from.get_alteration ());

  return pt.transposed (Pitch (0, 0, sound - pt.quartertone_pitch ()));
}

/* FIXME
   Merge with *pitch->text* funcs in chord-name.scm  */
char const *accname[] = {"eses", "eseh", "es", "eh", "",
			 "ih", "is", "isih", "isis"};

String
Pitch::to_string () const
{
  int n = (notename_ + 2) % 7;
  String s = ::to_string (char (n + 'a'));
  if (alteration_)
    s += String (accname[alteration_ - DOUBLE_FLAT]);

  if (octave_ >= 0)
    {
      int o = octave_ + 1;
      while (o--)
	s += "'";
    }
  else if (octave_ < 0)
    {
      int o = (-octave_) - 1;
      while (o--)
	s += ::to_string (',');
    }

  return s;
}

/* Change me to relative, counting from last pitch p
   return copy of resulting pitch.  */
Pitch
Pitch::to_relative_octave (Pitch p) const
{
  /* account for c' = octave 1 iso. 0 4 */
  int oct_mod = octave_ + 1;
  Pitch up_pitch (p);
  Pitch down_pitch (p);

  up_pitch.alteration_ = alteration_;
  down_pitch.alteration_ = alteration_;

  Pitch n = *this;
  up_pitch.up_to (notename_);
  down_pitch.down_to (notename_);

  int h = p.steps ();
  if (abs (up_pitch.steps () - h) < abs (down_pitch.steps () - h))
    n = up_pitch;
  else
    n = down_pitch;

  n.octave_ += oct_mod;
  return n;
}

void
Pitch::up_to (int notename)
{
  if (notename_ > notename)
    octave_++;
  notename_ = notename;
}

void
Pitch::down_to (int notename)
{
  if (notename_ < notename)
    octave_--;
  notename_ = notename;
}

IMPLEMENT_TYPE_P (Pitch, "ly:pitch?");

SCM
Pitch::mark_smob (SCM)
{
  return SCM_EOL;
}

IMPLEMENT_SIMPLE_SMOBS (Pitch);
int
Pitch::print_smob (SCM s, SCM port, scm_print_state *)
{
  Pitch *r = (Pitch *) SCM_CELL_WORD_1 (s);
  scm_puts ("#<Pitch ", port);
  scm_display (scm_makfrom0str (r->to_string ().to_str0 ()), port);
  scm_puts (" >", port);
  return 1;
}

SCM
Pitch::equal_p (SCM a, SCM b)
{
  Pitch *p = (Pitch *) SCM_CELL_WORD_1 (a);
  Pitch *q = (Pitch *) SCM_CELL_WORD_1 (b);

  bool eq = p->notename_ == q->notename_
    && p->octave_ == q->octave_
    && p->alteration_ == q->alteration_;

  return eq ? SCM_BOOL_T : SCM_BOOL_F;
}

MAKE_SCHEME_CALLBACK (Pitch, less_p, 2);
SCM
Pitch::less_p (SCM p1, SCM p2)
{
  Pitch *a = unsmob_pitch (p1);
  Pitch *b = unsmob_pitch (p2);

  if (compare (*a, *b) < 0)
    return SCM_BOOL_T;
  else
    return SCM_BOOL_F;
}

int
Pitch::get_octave () const
{
  return octave_;
}

int
Pitch::get_notename () const
{
  return notename_;
}

int
Pitch::get_alteration () const
{
  return alteration_;
}

Pitch
Pitch::transposed (Pitch d) const
{
  Pitch p = *this;
  p.transpose (d);
  return p;
}
