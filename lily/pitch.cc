/*
  pitch.cc -- implement Pitch

  source file of the GNU LilyPond music typesetter

  (c) 1998--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>
  2007--2008 Rune Zedeler <rune@zedeler.dk>
*/

#include "pitch.hh"

#include "main.hh"
#include "scale.hh"
#include "string-convert.hh"
#include "warn.hh"

#include "ly-smobs.icc"

Pitch::Pitch (Pitch const *p)
{
  notename_ = p->notename_;
  alteration_ = p->alteration_;
  octave_ = p->octave_;
  scale_ = p->scale_;
}

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

Pitchclass *
Pitch::clone () const
{
  return new Pitch (this);
}

string
Pitch::get_class_name () const
{
  return "Pitch";
}

int
Pitch::compare_to (Pitchclass const &m2) const
{
  const Pitch * m2p = dynamic_cast<const Pitch *> (&m2);
  if (m2p)
    {
      int o = octave_ - m2p->octave_;
      int n = notename_ - m2p->notename_;
      Rational a = alteration_ - m2p->alteration_;
      
      if (o)
	return o;
      if (n)
	return n;
      if (a)
	return a;
      
      return 0;
    }
  else
    {
      /* pitch comes after pitchclass */
      return 1;
    }
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
  Rational pitch = tone_pitch ();
  Pitchclass::normalize ();
  octave_ += ((pitch - tone_pitch ()) / Rational (6)).to_int ();
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
  s[0] = s[0]-'A'+'a';
  
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


Pitch *
unsmob_pitch (SCM s) {
  Pitchclass *pc = unsmob_pitchclass (s);
  Pitch *pitch = dynamic_cast<Pitch*> (pc);
  return pitch;
}

