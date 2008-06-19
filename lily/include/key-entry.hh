/*
  key-entry.hh -- declare Key_entry
  
  source file of the GNU LilyPond music typesetter

  (c) 2007-2008 Rune Zedeler <rune@zedeler.dk>
*/

#ifndef KEY_ENTRY_HH
#define KEY_ENTRY_HH

#include "lily-proto.hh"
#include "smobs.hh"
#include "moment.hh"
#include "rational.hh"
#include "pitch.hh"

/* 
FIXME!!!
Does it work with simple smobs when it contains a reference to a pitchclass_?
*/

class Key_entry
{
private:
  //Pitchclass * pitchclass_;
  SCM pitchclass_scm_;

  /* if false, this key_entry lasts until next key-sig-change */
  bool is_accidental_; 
  int bar_number_;
  Moment measure_position_;
  bool is_tied_;

public:
  string to_string () const;
  Pitchclass * get_pitchclass () const;
  SCM get_smobbed_pitchclass () const;
  bool is_accidental () const;
  int get_bar_number () const;
  Moment get_measure_position () const;
  bool is_tied () const;

  Key_entry (int notename, Rational alteration);
  Key_entry (int notename, int octave, 
		       int bar_number, Moment measure_position);
  Key_entry (int notename, Rational alteration, int octave, 
		       int bar_number, Moment measure_position);
  Key_entry (SCM scm);
  //Key_entry (Key_entry const * entry);

  Key_entry ();

  //~Key_entry();

  SCM to_name_alter_pair () const;

  static int compare (Key_entry const &, Key_entry const &);
  DECLARE_SIMPLE_SMOBS (Key_entry);
};


DECLARE_UNSMOB (Key_entry, key_entry);

INSTANTIATE_COMPARE (Key_entry, Key_entry::compare);

#endif /* KEY_ENTRY_HH */

