/*
  musical-pitch.cc -- implement Pitch

  source file of the GNU LilyPond music typesetter

  (c) 1998--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "pitch.hh"

#include "main.hh"
#include "scale.hh"
#include "string-convert.hh"
#include "warn.hh"

#include "ly-smobs.icc"

Pitch::Pitch (int o, int n, Rational a)
{
  notename_ = n;
  alteration_ = a;
  octave_ = o;
  scale_ = default_global_scale; 
  normalize ();
}

Pitch::Pitch ()
{
  notename_ = 0;
  scale_ = default_global_scale; 
  octave_ = 0;
}

int
Pitch::compare (Pitch const &m1, Pitch const &m2)
{
  int o = m1.octave_ - m2.octave_;
  int n = m1.notename_ - m2.notename_;
  Rational a = m1.alteration_ - m2.alteration_;

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
  return notename_ + octave_ * (int)scale_->step_tones_.size ();
}

Rational
Pitch::tone_pitch () const
{
  /*
    we're effictively hardcoding the octave to 6 whole-tones,
    which is as arbitrary as coding it to 1200 cents
  */
  Rational tones = scale_->pitch_at_step (octave_, notename_);
  tones += alteration_;  
  return tones;
}


void
Pitch::normalize ()
{
  //  cout << "normalizing " << octave_ << " " << notename_ << " " << alteration_.to_string() << " -> ";
  Rational pitch = tone_pitch ();
  Pitchclass::normalize ();
  octave_ += ((pitch - tone_pitch ()) / Rational (6)).to_int ();
  //  cout << octave_ << " " << notename_ << " " << alteration_.to_string() << endl;
}

void
Pitch::transpose (Pitch delta)
{
  Rational new_alter = tone_pitch () + delta.tone_pitch ();

  octave_ += delta.octave_;
  notename_ += delta.notename_;
  alteration_ += new_alter - tone_pitch ();

  normalize ();
}

Pitchclass *
unsmob_pitch_or_pitchclass (SCM s, int number)
{
  Pitchclass *pc = unsmob_pitch (s);
  if (pc==NULL)
    {
      LY_ASSERT_SMOB (Pitchclass, s, number);
      pc = unsmob_pitchclass (s);
    }
  return pc;
}

// TODO: Merge pitch_interval and pitchclass_interval
Pitch
pitch_interval (Pitch const &from, Pitch const &to)
{
  Rational sound = to.tone_pitch () - from.tone_pitch ();
  Pitch pt (to.get_octave () - from.get_octave (),
	    to.get_notename () - from.get_notename (),

	    to.get_alteration () - from.get_alteration ());

  return pt.transposed (Pitch (0, 0, sound - pt.tone_pitch ()));
}

string
Pitch::to_string () const
{
  string s = Pitchclass::to_string ();

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
Pitch::mark_smob (SCM x)
{
  Pitch *p = (Pitch*) SCM_CELL_WORD_1 (x);
  return p->scale_->self_scm ();
}

IMPLEMENT_SIMPLE_SMOBS (Pitch);
int
Pitch::print_smob (SCM s, SCM port, scm_print_state *)
{
  Pitch *r = (Pitch *) SCM_CELL_WORD_1 (s);
  scm_puts ("#<Pitch ", port);
  scm_display (ly_string2scm (r->to_string ()), port);
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

Pitch
Pitch::transposed (Pitch d) const
{
  Pitch p = *this;
  p.transpose (d);
  return p;
}

Pitchclass
Pitch::transposed (Pitchclass d) const
{
  Pitch * dp = dynamic_cast<Pitch *> (&d);
  if (dp)
    return transposed (*dp);
  else
    return Pitchclass::transposed (d);
}

Pitch
Pitch::negated () const
{
  return pitch_interval (*this, Pitch ());
}
