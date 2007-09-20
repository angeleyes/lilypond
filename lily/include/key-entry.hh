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

class Key_entry
{
private:
  bool has_octave_; // hmm. In ML one would say "octave_ : int option"
  int octave_;
  int notename_;
  Rational alteration_;
  bool has_position_;
  int bar_number_;
  Moment measure_position_;
  bool is_tied_;

  // todo: Perhaps add Scale *

public:
  string to_string () const;
  bool has_octave () const;
  int get_octave () const;
  int get_notename () const;
  Rational get_alteration () const;
  bool has_position () const;
  int get_bar_number () const;
  bool is_tied () const;
  Moment get_measure_position () const;

  Key_entry (int notename, Rational alteration);
  Key_entry (int notename, int octave, 
		       int bar_number, Moment measure_position);
  Key_entry (int notename, Rational alteration, int octave, 
		       int bar_number, Moment measure_position);
  Key_entry (SCM scm);
  // todo: add constructors for other combinations of has_octave and has_position
  Key_entry ();

  SCM to_name_alter_pair () const;

  static int compare (Key_entry const &, Key_entry const &);
  DECLARE_SIMPLE_SMOBS (Key_entry);
};


//DECLARE_UNSMOB (Key_entry, key_signature_entry);

INSTANTIATE_COMPARE (Key_entry, Key_entry::compare);

//extern SCM pitch_less_proc;

#endif /* MUSICAL_KEY_ENTRY_HH */

