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
  /* if the SCM contains a Pitch then it is considered a local accidental,
     if it contains a Pitchclass then it is considered part of the global keysig
     and hence lasts until the next keysig-change */
  SCM pitchclass_scm_;

  int bar_number_;
  Moment measure_position_;
  bool is_tied_;

public:
  string to_string () const;
  /* returns reference to the smobbed pitchclass.
     Unsafe - Will be gc'ed when the smob dies */
  Pitchclass * get_pitchclass_ref () const;
  SCM get_smobbed_pitchclass () const;
  bool is_accidental () const;
  int get_bar_number () const;
  Moment get_measure_position () const;
  bool is_tied () const;

  Key_entry ();

  void set_pitchclass (Pitchclass pc);
  void set_pitch (Pitch p);
  void set_position (int bar_number, Moment measure_position);
  void set_is_tied (bool=true);

  SCM to_name_alter_pair () const;
  static Key_entry from_name_alter_pair (SCM scm);

  static int compare (Key_entry const &, Key_entry const &);
  DECLARE_SIMPLE_SMOBS (Key_entry);
};


DECLARE_UNSMOB (Key_entry, key_entry);

INSTANTIATE_COMPARE (Key_entry, Key_entry::compare);

#endif /* KEY_ENTRY_HH */

