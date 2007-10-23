/*
  key-entry.hh -- declare TODO

  source file of the GNU LilyPond music typesetter

  (c) 2007 Rune Zedeler <rune@zedeler.dk>
*/

#ifndef MUSICAL_KEY_ENTRY_HH
#define MUSICAL_KEY_ENTRY_HH

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
  /* This has to be a reference -
     otherwise it cannot contain a pitch (subclass of pitchclass)
   */
  Pitchclass * pitchclass_;

  /* if false, this key_entry lasts until next key-sig-change */
  bool is_accidental_; 
  int bar_number_;
  Moment measure_position_;
  bool is_tied_;

public:
  string to_string () const;
  Pitchclass * get_pitchclass () const;
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
  // todo: add constructors for other combinations of has_octave and has_position
  Key_entry ();

  ~Key_entry();


  SCM to_name_alter_pair () const;

  static int compare (Key_entry const &, Key_entry const &);
  DECLARE_SIMPLE_SMOBS (Key_entry);
};


//DECLARE_UNSMOB (Key_entry, key_signature_entry);

INSTANTIATE_COMPARE (Key_entry, Key_entry::compare);

//extern SCM pitch_less_proc;

#endif /* MUSICAL_KEY_ENTRY_HH */

