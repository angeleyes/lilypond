/*
  accidental-engraver.cc -- implement accidental_engraver

  source file of the GNU LilyPond music typesetter

  (c) 1997--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>
  Modified 2001--2002 by Rune Zedeler <rz@daimi.au.dk>
*/

#include "accidental-placement.hh"
#include "arpeggio.hh"
#include "spanner.hh"
#include "context.hh"
#include "item.hh"
#include "engraver.hh"
#include "international.hh"
#include "pitch.hh"
#include "protected-scm.hh"
#include "rhythmic-head.hh"
#include "side-position-interface.hh"
#include "stream-event.hh"
#include "tie.hh"
#include "warn.hh"

#include "translator.icc"

class Accidental_entry
{
public:
  bool done_;
  Stream_event *melodic_;
  Grob *accidental_;
  Context *origin_;
  Engraver *origin_engraver_;
  Grob *head_;
  bool tied_;

  Accidental_entry ();
};

Accidental_entry::Accidental_entry ()
{
  tied_ = false;
  done_ = false;
  melodic_ = 0;
  accidental_ = 0;
  origin_ = 0;
  head_ = 0;
}

class Accidental_engraver : public Engraver
{
  int get_bar_number ();
  void update_local_key_signature (SCM new_signature);
  void create_accidental (Accidental_entry *entry, bool, bool);
  Grob *make_standard_accidental (Stream_event *note, Grob *note_head, Engraver *trans, bool);
  Grob *make_suggested_accidental (Stream_event *note, Grob *note_head, Engraver *trans);

protected:
  TRANSLATOR_DECLARATIONS (Accidental_engraver);
  void process_music ();

  void acknowledge_tie (Grob_info);
  void acknowledge_arpeggio (Grob_info);
  void acknowledge_rhythmic_head (Grob_info);
  void acknowledge_finger (Grob_info);

  void stop_translation_timestep ();
  void process_acknowledged ();
  
  virtual void finalize ();
  virtual void derived_mark () const;

public:
  SCM last_keysig_;

  vector<Grob*> left_objects_;
  vector<Grob*> right_objects_;

  Grob *accidental_placement_;

  vector<Accidental_entry> accidentals_;
  vector<Spanner*> ties_;
};

/*
  localKeySignature is changed at runtime, which means that references
  in grobs should always store ly_deep_copy ()s of those.
*/


Accidental_engraver::Accidental_engraver ()
{
  accidental_placement_ = 0;
  last_keysig_ = SCM_EOL;
}

void
Accidental_engraver::derived_mark () const
{
  scm_gc_mark (last_keysig_);
}

void
Accidental_engraver::update_local_key_signature (SCM new_sig)
{
  last_keysig_ = new_sig;
  set_context_property_on_children (context (),
				    ly_symbol2scm ("localKeySignature"),
				    new_sig);

  Context *trans = context ()->get_parent_context ();

  /* Reset parent contexts so that e.g. piano-accidentals won't remember old
     cross-staff accidentals after key-sig-changes */

  SCM val;
  while (trans && trans->where_defined (ly_symbol2scm ("localKeySignature"), &val)==trans)
    {
      trans->set_property ("localKeySignature", ly_deep_copy (last_keysig_));
      trans = trans->get_parent_context ();
    }
}


/** Calculate the number of accidentals on basis of the current local key
    sig (passed as argument)

    * First check step+octave (taking into account barnumbers if necessary).

    * Then check the global signature (only step).

    Return number of accidentals (0, 1 or 2).  */

static bool
recent_enough (int bar_number, SCM alteration_def, SCM laziness)
{
  if (scm_is_number (alteration_def)
      || laziness == SCM_BOOL_T)
    return true;

  return (bar_number <= scm_to_int (scm_cdr (alteration_def)) + scm_to_int (laziness));
}

static Rational
extract_alteration (SCM alteration_def)
{
  if (scm_is_number (alteration_def))
    return ly_scm2rational (alteration_def);
  else if (scm_is_pair (alteration_def))
    return ly_scm2rational (scm_car (alteration_def));
  else if (alteration_def == SCM_BOOL_F)
    return Rational (0);
  else
    assert (0);
  return Rational (0);
}

bool
is_tied (SCM alteration_def)
{
  SCM tied = ly_symbol2scm ("tied");
  return (alteration_def == tied
	  || (scm_is_pair (alteration_def) && scm_car (alteration_def) == tied));
}

struct Accidental_result
{
  bool need_acc;
  bool need_restore;

  Accidental_result () {
    need_restore = need_acc = false;
  }

  int score () const {
    return need_acc ? 1 : 0
      + need_restore ? 1 : 0;
  }
};

Accidental_result
check_pitch_against_signature (SCM key_signature, Pitch const &pitch,
			       int bar_number, SCM laziness, bool ignore_octave)
{
  Accidental_result result;
  int n = pitch.get_notename ();
  int o = pitch.get_octave ();

  SCM previous_alteration = SCM_BOOL_F;

  SCM from_same_octave = ly_assoc_get (scm_cons (scm_from_int (o),
						 scm_from_int (n)), key_signature, SCM_BOOL_F);
  SCM from_key_signature = ly_assoc_get (scm_from_int (n), key_signature, SCM_BOOL_F);
  SCM from_other_octaves = SCM_BOOL_F;
  for (SCM s = key_signature; scm_is_pair (s); s = scm_cdr (s))
    {
      SCM entry = scm_car (s);
      if (scm_is_pair (scm_car (entry))
	  && scm_cdar (entry) == scm_from_int (n))
	{
	  from_other_octaves = scm_cdr (entry);
	  break;
	}
    }

  if (!ignore_octave
      && from_same_octave != SCM_BOOL_F
      && recent_enough (bar_number, from_same_octave, laziness))
    previous_alteration = from_same_octave;
  else if (ignore_octave
	   && from_other_octaves != SCM_BOOL_F
	   && recent_enough (bar_number, from_other_octaves, laziness))
    previous_alteration = from_other_octaves;
  else if (from_key_signature != SCM_BOOL_F)
    previous_alteration = from_key_signature;

  if (is_tied (previous_alteration))
    {
      result.need_acc = true;
    }
  else
    {
      Rational prev = extract_alteration (previous_alteration);
      Rational alter = pitch.get_alteration ();

      if (alter != prev)
        {
	  result.need_acc = true;
	  if (alter.sign ()
	      && (alter.abs () < prev.abs ()
		  || (prev * alter).sign () < 0))
	    result.need_restore = true;
	}
    }

  return result;
}

static
Accidental_result
check_pitch_against_rules (Pitch const &pitch, Context *origin,
				 SCM rules, int bar_number)
{
  Accidental_result result;
  if (scm_is_pair (rules) && !scm_is_symbol (scm_car (rules)))
    warning (_f ("accidental typesetting list must begin with context-name: %s",
		 ly_scm2string (scm_car (rules)).c_str ()));

  for (; scm_is_pair (rules) && origin;
       rules = scm_cdr (rules))
    {
      SCM rule = scm_car (rules);
      if (scm_is_pair (rule))
	{
	  SCM type = scm_car (rule);
	  SCM laziness = scm_cdr (rule);
	  SCM localsig = origin->get_property ("localKeySignature");

	  bool same_octave
	    = (ly_symbol2scm ("same-octave") == type);
	  bool any_octave
	    = (ly_symbol2scm ("any-octave") == type);

	  if (same_octave || any_octave)
	    {
	      Accidental_result rule_result = check_pitch_against_signature
		(localsig, pitch, bar_number, laziness, any_octave);

	      result.need_acc |= rule_result.need_acc;
	      result.need_restore |= rule_result.need_restore;
	    }
	  else
	    warning (_f ("ignoring unknown accidental rule: %s",
			 ly_symbol2string (type).c_str ()));
	}

      /* if symbol then it is a context name.  Scan parent contexts to
	 find it. */
      else if (scm_is_symbol (rule))
	{
	  Context *dad = origin;
	  while (dad && !dad->is_alias (rule))
	    dad = dad->get_parent_context ();

	  if (dad)
	    origin = dad;
	}
      else
	warning (_f ("pair or context-name expected for accidental rule, found %s",
		     ly_scm2string (rule).c_str ()));
    }

  return result;
}

int
Accidental_engraver::get_bar_number ()
{
  SCM barnum = get_property ("internalBarNumber");
  SCM smp = get_property ("measurePosition");

  int bn = robust_scm2int (barnum, 0);

  Moment mp = robust_scm2moment (smp, Moment (0));
  if (mp.main_part_ < Rational (0))
    bn--;

  return bn;
}

void
Accidental_engraver::process_acknowledged ()
{
  if (accidentals_.size () && !accidentals_.back ().done_)
    {
      SCM accidental_rules = get_property ("autoAccidentals");
      SCM cautionary_rules = get_property ("autoCautionaries");
      int barnum = get_bar_number ();

      for (vsize i = 0; i < accidentals_.size (); i++)
	{
	  if (accidentals_[i].done_)
	    continue;
	  accidentals_[i].done_ = true;

	  Stream_event *note = accidentals_[i].melodic_;
	  Context *origin = accidentals_[i].origin_;

	  Pitch *pitch = unsmob_pitch (note->get_property ("pitch"));
	  if (!pitch)
	    continue;

	  Accidental_result acc = check_pitch_against_rules (*pitch, origin,
							     accidental_rules, barnum);
	  Accidental_result caut = check_pitch_against_rules (*pitch, origin,
							      cautionary_rules, barnum);

	  bool cautionary = to_boolean (note->get_property ("cautionary"));
	  if (caut.score () > acc.score ())
	    {
	      acc.need_acc |= caut.need_acc; 
	      acc.need_restore |= caut.need_restore; 

	      cautionary = true;
	    }

	  bool forced = to_boolean (note->get_property ("force-accidental"));
	  if (!acc.need_acc && forced)
	    acc.need_acc = true;

	  /* Cannot look for ties: it's not guaranteed that they reach
	     us before the notes. */
	  if (acc.need_acc
	      && !note->in_event_class ("trill-span-event"))
	    create_accidental (&accidentals_[i], acc.need_restore, cautionary);

	  if (forced || cautionary)
	    accidentals_[i].accidental_->set_property ("forced", SCM_BOOL_T);
	}
    }
}

void
Accidental_engraver::create_accidental (Accidental_entry *entry,
					bool restore_natural,
					bool cautionary)
{
  Stream_event *note = entry->melodic_;
  Grob *support = entry->head_;
  bool as_suggestion = to_boolean (entry->origin_->get_property ("suggestAccidentals"));
  Grob *a = 0;
  if (as_suggestion)
    a = make_suggested_accidental (note, support, entry->origin_engraver_);
  else
    a = make_standard_accidental (note, support, entry->origin_engraver_, cautionary);

  if (restore_natural)
    {
      if (to_boolean (get_property ("extraNatural")))
	a->set_property ("restore-first", SCM_BOOL_T);
    }

  entry->accidental_ = a;
}

Grob *
Accidental_engraver::make_standard_accidental (Stream_event *note,
					       Grob *note_head,
					       Engraver *trans,
					       bool cautionary)
{
  (void)note;

  /*
    We construct the accidentals at the originating Voice
    level, so that we get the property settings for
    Accidental from the respective Voice.
  */
  Grob *a = 0;
  if (cautionary)
    a = trans->make_item ("AccidentalCautionary", note_head->self_scm ());
  else
    a = trans->make_item ("Accidental", note_head->self_scm ());

  /*
    We add the accidentals to the support of the arpeggio,
    so it is put left of the accidentals.
  */
  for (vsize i = 0; i < left_objects_.size (); i++)
    {
      if (left_objects_[i]->get_property ("side-axis") == scm_from_int (X_AXIS))
	Side_position_interface::add_support (left_objects_[i], a);
    }

  for (vsize i = 0; i < right_objects_.size (); i++)
    Side_position_interface::add_support (a, right_objects_[i]);

  a->set_parent (note_head, Y_AXIS);

  if (!accidental_placement_)
    accidental_placement_ = make_item ("AccidentalPlacement",
				       a->self_scm ());
  Accidental_placement::add_accidental (accidental_placement_, a);

  note_head->set_object ("accidental-grob", a->self_scm ());
  
  return a;
}

Grob *
Accidental_engraver::make_suggested_accidental (Stream_event *note,
						Grob *note_head,
						Engraver *trans)
{
  (void) note;

  Grob *a = trans->make_item ("AccidentalSuggestion", note_head->self_scm ());

  Side_position_interface::add_support (a, note_head);
  if (Grob *stem = unsmob_grob (a->get_object ("stem")))
    Side_position_interface::add_support (a, stem);

  a->set_parent (note_head, X_AXIS);
  return a;
}

void
Accidental_engraver::finalize ()
{
  last_keysig_ = SCM_EOL;
}

void
Accidental_engraver::stop_translation_timestep ()
{
  for (vsize j = ties_.size (); j--;)
    {
      Grob *r = Tie::head (ties_[j], RIGHT);
      for (vsize i = accidentals_.size (); i--;)
	if (accidentals_[i].head_ == r)
	  {
	    if (Grob *g = accidentals_[i].accidental_)
	      {
		g->set_object ("tie", ties_[j]->self_scm ());
		accidentals_[i].tied_ = true;
	      }
	    ties_.erase (ties_.begin () + j);
	    break;
	  }
    }

  for (vsize i = accidentals_.size (); i--;)
    {
      int barnum = get_bar_number ();

      Stream_event *note = accidentals_[i].melodic_;
      Context *origin = accidentals_[i].origin_;

      Pitch *pitch = unsmob_pitch (note->get_property ("pitch"));
      if (!pitch)
	continue;

      int n = pitch->get_notename ();
      int o = pitch->get_octave ();
      Rational a = pitch->get_alteration ();
      SCM key = scm_cons (scm_from_int (o), scm_from_int (n));

      SCM localsig = SCM_EOL;
      while (origin
	     && origin->where_defined (ly_symbol2scm ("localKeySignature"), &localsig))
	{
	  bool change = false;
	  if (accidentals_[i].tied_)
	    {
	      /*
		Remember an alteration that is different both from
		that of the tied note and of the key signature.
	      */
	      localsig = ly_assoc_prepend_x (localsig, key, scm_cons (ly_symbol2scm ("tied"),
								      scm_from_int (barnum)));

	      change = true;
	    }
	  else
	    {
	      /*
		not really really correct if there are more than one
		noteheads with the same notename.
	      */
	      localsig = ly_assoc_prepend_x (localsig, key,
					   scm_cons (ly_rational2scm (a),
						     scm_from_int (barnum)));
	      change = true;
	    }

	  if (change)
	    origin->set_property ("localKeySignature", localsig);

	  origin = origin->get_parent_context ();
	}
    }

  accidental_placement_ = 0;
  accidentals_.clear ();
  left_objects_.clear ();
  right_objects_.clear ();
}

void
Accidental_engraver::acknowledge_rhythmic_head (Grob_info info)
{
  Stream_event *note = info.event_cause ();
  if (note
      && (note->in_event_class ("note-event")
	  || note->in_event_class ("trill-span-event")))
    {
      /*
	string harmonics usually don't have accidentals.
      */
      if (info.grob ()->get_property ("style") != ly_symbol2scm ("harmonic")
	  || to_boolean (get_property ("harmonicAccidentals")))
	{
	  Accidental_entry entry;
	  entry.head_ = info.grob ();
	  entry.origin_engraver_ = dynamic_cast<Engraver *> (info.origin_translator ());
	  entry.origin_ = entry.origin_engraver_->context ();
	  entry.melodic_ = note;

	  accidentals_.push_back (entry);
	}
    }
}

void
Accidental_engraver::acknowledge_tie (Grob_info info)
{
  ties_.push_back (dynamic_cast<Spanner *> (info.grob ()));
}

void
Accidental_engraver::acknowledge_arpeggio (Grob_info info)
{
  left_objects_.push_back (info.grob ());
}

void
Accidental_engraver::acknowledge_finger (Grob_info info)
{
  left_objects_.push_back (info.grob ());
}

void
Accidental_engraver::process_music ()
{
  SCM sig = get_property ("keySignature");
  if (last_keysig_ != sig)
    update_local_key_signature (sig);
}

ADD_ACKNOWLEDGER (Accidental_engraver, arpeggio);
ADD_ACKNOWLEDGER (Accidental_engraver, finger);
ADD_ACKNOWLEDGER (Accidental_engraver, rhythmic_head);
ADD_ACKNOWLEDGER (Accidental_engraver, tie);

ADD_TRANSLATOR (Accidental_engraver,
		
		"Make accidentals.  "
		"Catch note heads, ties and notices key-change events.  "
		"This engraver usually lives at Staff level, but "
		"reads the settings for Accidental at @code{Voice} level, "
		"so you can @code{\\override} them at @code{Voice}. ",

		/* grobs */
		"Accidental "
		"AccidentalCautionary "
		"AccidentalSuggestion ",

		/* props */
		"autoAccidentals "
		"autoCautionaries "
		"internalBarNumber "
		"extraNatural "
		"harmonicAccidentals "
		"localKeySignature ",
		"localKeySignature "
		);