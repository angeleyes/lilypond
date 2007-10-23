/*
  pitchclass.cc -- implement Pitchclass

  source file of the GNU LilyPond music typesetter

  (c) 2007 Rune Zedeler <rune@zedeler.dk>
*/

#include "pitchclass.hh"

#include "main.hh"
#include "scale.hh"
#include "string-convert.hh"
#include "warn.hh"

#include "ly-smobs.icc"

#include <iostream>

Pitchclass::Pitchclass (int n, Rational a)
{
  notename_ = n;
  alteration_ = a;
  scale_ = default_global_scale; 
  normalize ();
}

Pitchclass::Pitchclass ()
{
  notename_ = 0;
  alteration_ = 0;
  scale_ = default_global_scale; 
}

int
Pitchclass::compare (Pitchclass const &m1, Pitchclass const &m2)
{
  int n = m1.notename_ - m2.notename_;
  Rational a = m1.alteration_ - m2.alteration_;

  if (n)
    return n;
  if (a)
    return a;
  
  return 0;
}

Rational
Pitchclass::tone_pitch () const
{
  /*
    we're effictively hardcoding the octave to 6 whole-tones,
    which is as arbitrary as coding it to 1200 cents
  */
  Rational tones = scale_->pitch_at_step (notename_);
  tones += alteration_;
  tones %= Rational(6);
  if(tones < Rational(0)) 
    tones += Rational(6);
  
  return tones;
}

/* Calculate pitch height in 12th octave steps.  Don't assume
   normalized pitch as this function is used to normalize the pitch.  */
int
Pitchclass::rounded_semitone_pitch () const
{
  return int (round (double (tone_pitch () * Rational (2))));
}

int
Pitchclass::rounded_quartertone_pitch () const
{
  return int (round (double (tone_pitch () * Rational (4))));
}


void
Pitchclass::normalize ()
{
  /* Don't allow alterations on more than half an octave */
  alteration_ = ((alteration_ + Rational (3)) % Rational (6)) - Rational (3);
  while (alteration_ > Rational (1))
    {
      notename_++;
      alteration_ += scale_->pitch_at_step(notename_-1)
	- scale_->pitch_at_step(notename_);
    }
  while (alteration_ < Rational (-1))
    {
      notename_--;
      alteration_ += scale_->pitch_at_step(notename_+1)
	- scale_->pitch_at_step(notename_);
    }
  notename_ %= (int)scale_->step_tones_.size ();
  if(notename_ < 0)
    notename_ += scale_->step_tones_.size ();
}

void
Pitchclass::transpose (Pitchclass delta)
{
  Rational new_alter = tone_pitch () + delta.tone_pitch ();

  notename_ += delta.notename_;
  alteration_ += new_alter - tone_pitch ();

  normalize ();
}

Pitchclass
pitchclass_interval (Pitchclass const &from, Pitchclass const &to)
{
  Rational sound = to.tone_pitch () - from.tone_pitch ();
  Pitchclass pc (to.get_notename () - from.get_notename (),
		 
		 to.get_alteration () - from.get_alteration ());

  return pc.transposed (Pitchclass (0, sound - pc.tone_pitch ()));
}

/* FIXME
   Merge with *pitch->text* funcs in chord-name.scm  */
char const *accname[] = {"eses", "eseh", "es", "eh", "",
			 "ih", "is", "isih", "isis"};

string
Pitchclass::to_string () const
{
  int n = (notename_ + 2) % (int)scale_->step_tones_.size ();
  string s = ::to_string (char (n + 'a'));
  Rational qtones = alteration_ * Rational (4,1);
  int qt = int (rint (Real (qtones)));
      
  s += string (accname[qt + 4]);
  return s;
}


IMPLEMENT_TYPE_P (Pitchclass, "ly:pitchclass?");
SCM
Pitchclass::mark_smob (SCM x)
{
  Pitchclass *p = (Pitchclass*) SCM_CELL_WORD_1 (x);
  return p->scale_->self_scm ();
}

IMPLEMENT_SIMPLE_SMOBS (Pitchclass);
int
Pitchclass::print_smob (SCM s, SCM port, scm_print_state *)
{
  Pitchclass *r = (Pitchclass *) SCM_CELL_WORD_1 (s);
  scm_puts ("#<Pitchclass ", port);
  scm_display (ly_string2scm (r->to_string ()), port);
  scm_puts (" >", port);
  return 1;
}

SCM
Pitchclass::equal_p (SCM a, SCM b)
{
  Pitchclass *p = (Pitchclass *) SCM_CELL_WORD_1 (a);
  Pitchclass *q = (Pitchclass *) SCM_CELL_WORD_1 (b);

  bool eq = p->notename_ == q->notename_
    && p->alteration_ == q->alteration_;

  return eq ? SCM_BOOL_T : SCM_BOOL_F;
}

MAKE_SCHEME_CALLBACK (Pitchclass, less_p, 2);
SCM
Pitchclass::less_p (SCM p1, SCM p2)
{
  Pitchclass *a = unsmob_pitchclass (p1);
  Pitchclass *b = unsmob_pitchclass (p2);

  if (compare (*a, *b) < 0)
    return SCM_BOOL_T;
  else
    return SCM_BOOL_F;
}

int
Pitchclass::get_notename () const
{
  return notename_;
}

Rational
Pitchclass::get_alteration () const
{
  cout << "getting alteration ... ";
  Rational result = alteration_;
  cout << "Done" << endl;
  return result;
}

Pitchclass
Pitchclass::transposed (Pitchclass d) const
{
  Pitchclass p = *this;
  p.transpose (d);
  return p;
}

Rational NATURAL_ALTERATION (0);
Rational FLAT_ALTERATION (-1, 2);
Rational DOUBLE_FLAT_ALTERATION (-1);
Rational SHARP_ALTERATION (1, 2);

Pitchclass
Pitchclass::negated () const
{
  return pitchclass_interval (*this, Pitchclass ());
}
