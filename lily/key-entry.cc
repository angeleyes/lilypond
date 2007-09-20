/*
  ...
*/

#include "key-entry.hh"

#include "main.hh"
#include "string-convert.hh"
#include "international.hh"
#include "warn.hh"
#include "pitch.hh"
#include "rational.hh"
#include "moment.hh"

#include "ly-smobs.icc"


Key_entry::Key_entry (int n, Rational a)
{
  notename_ = n;
  alteration_ = a;
  has_octave_ = false;
  has_position_ = false;
  is_tied_ = false;
}

Key_entry::Key_entry (int n, Rational a, int o, int b, Moment mp)
{
  notename_ = n;
  alteration_ = a;
  octave_ = o;
  bar_number_ = b;
  measure_position_ = mp;
  has_octave_ = true;
  has_position_ = true;
  is_tied_ = false;
}

Key_entry::Key_entry (int n, int o, int b, Moment mp)
{
  notename_ = n;
  octave_ = o;
  bar_number_ = b;
  measure_position_ = mp;
  has_octave_ = true;
  has_position_ = true;
  is_tied_ = true;
}

Key_entry::Key_entry (SCM scm)
{
  assert (scm_is_pair (scm));
  assert (scm_is_int (scm_car (scm)));
  assert (scm_is_num (scm_cdr (scm)));
  int n = scm_to_int (scm_car (scm));
  Rational a = ly_scm2rational (scm_cdr (scm));
  //  ::Key_entry (n, a);
  notename_ = n;
  alteration_ = a;
  has_octave_ = false;
  has_position_ = false;
  is_tied_ = false;
}

Key_entry::Key_entry ()
{
  notename_ = 0;
  alteration_ = 0;
  has_octave_ = false;
  has_position_ = false;
  is_tied_ = false;
}



IMPLEMENT_TYPE_P (Key_entry, "ly:key-signature-entry?");

SCM
Key_entry::mark_smob (SCM x)
{
  return SCM_EOL; // FIXME: I don't understand what this function is supposed to do. This is just blindly coppied from moment.cc
}

IMPLEMENT_SIMPLE_SMOBS (Key_entry);
int
Key_entry::print_smob (SCM s, SCM port, scm_print_state *)
{
  Key_entry *r = (Key_entry *) SCM_CELL_WORD_1 (s);
  scm_puts ("#<Key_entry ", port);
  scm_display (ly_string2scm (r->to_string ()), port);
  scm_puts (" >", port);
  return 1;
}

SCM
Key_entry::equal_p (SCM a, SCM b)
{
  Key_entry *p = (Key_entry *) SCM_CELL_WORD_1 (a);
  Key_entry *q = (Key_entry *) SCM_CELL_WORD_1 (b);

  bool eq = p->notename_ == q->notename_
    && p->alteration_ == q->alteration_
    && p->has_position_ == q->has_position_
    && p->has_octave_ == q->has_octave_;
  if(eq && p->has_octave_)
    eq &= p->octave_ == q->octave_;
  if(eq && p->has_position_)
    eq &= p->bar_number_ == q->bar_number_
      && p->measure_position_ == q->measure_position_;

  return eq ? SCM_BOOL_T : SCM_BOOL_F;
}

string
Key_entry::to_string () const
{
  Pitch p = Pitch(has_octave_ ? octave_ : -1, notename_, alteration_);
  string pitchstring = p.to_string();
  return pitchstring;
}

SCM
Key_entry::to_name_alter_pair () const
{
  if (has_position_ || has_octave_ || is_tied_)
    warning(_ ("converting local keysig to name_alter_pair"));
  return scm_cons (scm_from_int (notename_), ly_rational2scm (alteration_));
}

bool
Key_entry::has_octave () const
{
  return has_octave_;
}

int
Key_entry::get_octave () const
{
  if(!has_octave_)
    warning(_ ("getting non-existent octave"));
  return octave_;
}

int
Key_entry::get_notename () const
{
  return notename_;
}

Rational
Key_entry::get_alteration () const
{
  return alteration_;
}

bool
Key_entry::has_position () const
{
  return has_position_;
}

bool
Key_entry::is_tied () const
{
  return is_tied_;
}

int
Key_entry::get_bar_number () const
{
  if(!has_position_)
    warning(_ ("getting non-existent bar_number"));
  return bar_number_;
}

Moment
Key_entry::get_measure_position () const
{
  if(!has_position_)
    warning(_ ("getting non-existent measure_position"));
  return measure_position_;
}

/* SCHEME **************************************************/


LY_DEFINE (ly_make_keysig_entry, "ly:make-keysig-entry",
	   2, 3, 0, (SCM note, SCM alter, SCM octave, SCM barnumber, SCM measurepos),
	   "An entry for key signature. Key signatures consists of lists of these. The three last arguments are used for local key changes.")
{
  LY_ASSERT_TYPE (scm_is_integer, note, 1);
  LY_ASSERT_TYPE (scm_is_integer, alter, 2);

  if (octave == SCM_UNDEFINED)
    {
      Key_entry e (scm_to_int (note), scm_to_int (alter));
      
      return e.smobbed_copy ();
    }
  else 
    {
      LY_ASSERT_TYPE (scm_is_integer, octave, 3);
      LY_ASSERT_TYPE (scm_is_integer, barnumber, 4);
      LY_ASSERT_SMOB (Moment, measurepos, 5);
      
      Key_entry e (scm_to_int (octave),
		   scm_to_int (note),
		   scm_to_int (alter),
		   scm_to_int (barnumber),
		   * unsmob_moment (measurepos));
      
      return e.smobbed_copy ();
    }
}
