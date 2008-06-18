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
  pitchclass_ = new Pitchclass (n, a);
  bar_number_ = 0;
  measure_position_ = Rational(0);
  is_accidental_ = false;
  is_tied_ = false;
}

Key_entry::Key_entry (int n, Rational a, int o, int b, Moment mp)
{
  pitchclass_ = new Pitch (o, n, a);
  bar_number_ = b;
  measure_position_ = mp;
  is_accidental_ = true;
  is_tied_ = false;
}

Key_entry::Key_entry (int n, int o, int b, Moment mp)
{
  pitchclass_ = new Pitch (o, n, Rational (0));
  bar_number_ = b;
  measure_position_ = mp;
  is_accidental_ = true;
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
  pitchclass_ = new Pitchclass (n, a);
  bar_number_ = 0;
  measure_position_ = Rational(0);
  is_accidental_ = false;
  is_tied_ = false;
}

Key_entry::Key_entry ()
{
  pitchclass_ = new Pitchclass ();
  bar_number_ = 0;
  measure_position_ = Rational(0);
  is_accidental_ = false;
  is_tied_ = false;
}

Key_entry::~Key_entry ()
{
  // TODO: what here?
  //cout << "Killing entry" << endl;
 
}

IMPLEMENT_TYPE_P (Key_entry, "ly:key-signature-entry?");

SCM
Key_entry::mark_smob (SCM x)
{
  return SCM_EOL;
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

  bool eq = *(p->pitchclass_) == *(q->pitchclass_)
    && p->is_accidental_ == q->is_accidental_
    && p->is_tied_ == q->is_tied_
    && p->bar_number_ == q->bar_number_
    && p->measure_position_ == q->measure_position_;

  return eq ? SCM_BOOL_T : SCM_BOOL_F;
}

string
Key_entry::to_string () const
{
  return pitchclass_->to_string ();
}

SCM
Key_entry::to_name_alter_pair () const
{
  if (dynamic_cast<Pitch *>(pitchclass_))
    warning(_ ("converting local keysig to name_alter_pair"));
  return scm_cons (scm_from_int (pitchclass_->get_notename()),
		   ly_rational2scm (pitchclass_->get_alteration()));
}

Pitchclass *
Key_entry::get_pitchclass () const
{
  return pitchclass_;
}

bool
Key_entry::is_accidental () const
{
  return is_accidental_;
}

bool
Key_entry::is_tied () const
{
  return is_tied_;
}

int
Key_entry::get_bar_number () const
{
  if(!is_accidental_)
    warning(_ ("getting non-existent bar_number"));
  return bar_number_;
}

Moment
Key_entry::get_measure_position () const
{
  if(!is_accidental_)
    warning(_ ("getting non-existent measure_position"));
  return measure_position_;
}

/* SCHEME **************************************************/


LY_DEFINE (ly_make_key_entry, "ly:make-key-entry",
	   2, 3, 0, (SCM note, SCM alter,
		     SCM octave, SCM barnumber, SCM measurepos),
	   "An entry for key signature. Key signatures consists of lists of"
	   "these. The three last arguments are used for local key changes.")
{
  LY_ASSERT_TYPE (scm_is_integer, note, 1);
  LY_ASSERT_TYPE (scm_is_rational, alter, 2);

  if (octave == SCM_BOOL_F)
    {
      Key_entry e (scm_to_int (note), ly_scm2rational (alter));
      
      return e.smobbed_copy ();
    }
  else 
    {
      LY_ASSERT_TYPE (scm_is_integer, octave, 3);
      LY_ASSERT_TYPE (scm_is_integer, barnumber, 4);
      LY_ASSERT_TYPE (unsmob_moment, measurepos, 5);
      
      Key_entry e (scm_to_int (octave),
		   scm_to_int (note),
		   scm_to_int (alter),
		   scm_to_int (barnumber),
		   * unsmob_moment (measurepos));
      
      return e.smobbed_copy ();
    }
}


/*
LY_DEFINE (ly_key_entry_octave, "ly:key-entry-octave",
	   1, 0, 0, (SCM entry),
	   "Extract octave information from key entry. #f if entry is global.")
{
  LY_ASSERT_SMOB (Key_entry, entry, 1);

  Key_entry *ent = unsmob_key_entry (entry);

  return ly_pitch_octave (ent->pitchclass_);
}

LY_DEFINE (ly_key_entry_notename, "ly:key-entry-notename",
	   1, 0, 0, (SCM entry),
	   "Get the notename that the key entry alters.")
{
  LY_ASSERT_SMOB (Key_entry, entry, 1);

  Key_entry *ent = unsmob_key_entry (entry);

  return ly_pitch_notename (ent->pitchclass_);
}

LY_DEFINE (ly_key_entry_octave, "ly:key-entry-accidental",
	   1, 0, 0, (SCM entry),
	   "Get the alteration of the key entry. 0 is natural, 1/2 is sharp, etc.")
{
  LY_ASSERT_SMOB (Key_entry, entry, 1);

  Key_entry *ent = unsmob_key_entry (entry);

  return ly_pitch_accidental (ent->pitchclass_);
}
*/

LY_DEFINE (ly_key_entry_pitch, "ly:key-entry-pitch",
	   1, 0, 0, (SCM entry),
	   "The pitch or pitchclass that the key entry defines.")
{
  LY_ASSERT_SMOB (Key_entry, entry, 1);

  Key_entry *ent = unsmob_key_entry (entry);

  return ent->get_pitchclass()->smobbed_clone();
}


LY_DEFINE (ly_key_entry_bar_number, "ly:key-entry-bar-number",
	   1, 0, 0, (SCM entry),
	   "Get the bar number in which the key entry occured.")
{
  LY_ASSERT_SMOB (Key_entry, entry, 1);

  Key_entry *ent = unsmob_key_entry (entry);

  if (ent->is_accidental ())
    return scm_from_int (ent->get_bar_number ());
  else
    return SCM_BOOL_F;
}

LY_DEFINE (ly_key_entry_measure_position, "ly:key-entry-measure-position",
	   1, 0, 0, (SCM entry),
	   "Extract octave information from key entry. #f if entry is global.")
{
  LY_ASSERT_SMOB (Key_entry, entry, 1);

  Key_entry *ent = unsmob_key_entry (entry);

  if (ent->is_accidental ())
    return ent->get_measure_position ().smobbed_copy ();
  else
    return SCM_BOOL_F;
}


