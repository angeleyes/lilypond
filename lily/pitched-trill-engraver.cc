/*
  pitched-trill-engraver.cc -- implement Pitched_trill_engraver

  source file of the GNU LilyPond music typesetter

  (c) 2005 Han-Wen Nienhuys <hanwen@xs4all.nl>

*/

#include "engraver.hh"

#include "dots.hh"
#include "group-interface.hh"
#include "axis-group-interface.hh"
#include "context.hh"
#include "note-head.hh"
#include "item.hh"
#include "side-position-interface.hh"
#include "pitch.hh"		
#include "warn.hh"

class Pitched_trill_engraver : public Engraver
{

public:
  TRANSLATOR_DECLARATIONS(Pitched_trill_engraver);
  
protected:
  virtual void acknowledge_grob (Grob_info);
  virtual void process_music ();
  virtual bool try_music (Music*);
  virtual void stop_translation_timestep ();
private:
  Item *trill_head_;
  Item *trill_group_;
  Item *trill_accidental_;

  Link_array<Grob> heads_;

  void make_trill (Music *);
};


Pitched_trill_engraver::Pitched_trill_engraver ()
{
  trill_head_ = 0;
  trill_group_ = 0;
  trill_accidental_ = 0;
}

void
Pitched_trill_engraver::acknowledge_grob (Grob_info info)
{
  Music *mus = info.music_cause ();

  if (Note_head::has_interface (info.grob ())
      || Dots::has_interface (info.grob ()))
    {
      heads_.push (info.grob ());
    }
  else if (mus
	   && mus->is_mus_type ("trill-span-event")
	   && to_dir (mus->get_property ("span-direction")) == START
	   && unsmob_pitch (mus->get_property ("trill-pitch")))
    {
      make_trill (mus);
    }
}

void
Pitched_trill_engraver::make_trill (Music *mus)
{
  SCM scm_pitch = mus->get_property ("trill-pitch");
  Pitch * p = unsmob_pitch (scm_pitch);

  SCM keysig = get_property ("localKeySignature");

  SCM key = scm_cons (scm_int2num (p->get_octave ()),
		      scm_int2num (p->get_notename ()));

  SCM handle = scm_assoc (key, keysig);
  bool print_acc =
    (handle == SCM_BOOL_F)
    || p->get_alteration () == 0;

  if (trill_head_)
    {
      programming_error ("already have a trill head.");
      trill_head_ = 0; 
    }

  trill_head_ = make_item ("TrillPitchHead", mus->self_scm ());
  SCM c0scm = get_property ("middleCPosition");

  int c0 = scm_is_number (c0scm) ? scm_to_int (c0scm) : 0;

  trill_head_->set_property ("staff-position",
			     scm_from_int (unsmob_pitch (scm_pitch)->steps ()
					   + c0));
  
  trill_group_ = make_item ("TrillPitchGroup", mus->self_scm());

  Axis_group_interface::add_element (trill_group_,  trill_head_);

  if (print_acc)
    {
      trill_accidental_ = make_item ("TrillPitchAccidental", mus->self_scm ());

      // fixme: naming -> alterations
      trill_accidental_->set_property ("accidentals", scm_list_1 (scm_from_int (p->get_alteration ())));
      Side_position_interface::add_support (trill_accidental_, trill_head_);
      trill_head_->set_property ("accidental-grob", trill_accidental_->self_scm ());
      trill_group_->set_parent (trill_head_, Y_AXIS);
      Axis_group_interface::add_element (trill_group_, trill_accidental_);
      trill_accidental_->set_parent (trill_head_, Y_AXIS);
    }
}

void
Pitched_trill_engraver::stop_translation_timestep ()
{
  if (trill_group_)
    for (int i = 0; i < heads_.size (); i++)
      {
	Side_position_interface::add_support (trill_group_, heads_[i]);
      }
  
  heads_.clear ();
  trill_head_ = 0;
  trill_group_ = 0;
  trill_accidental_ = 0;
}

void
Pitched_trill_engraver::process_music ()
{
}

bool
Pitched_trill_engraver::try_music (Music *)
{
  return false;
}

ADD_TRANSLATOR (Pitched_trill_engraver,
		/* descr */ "Print the bracketed notehead after a notehead with trill.",
		/* creats*/ "TrillPitchHead TrillPitchAccidental TrillPitchGroup",
		/* accepts */ "",
		/* acks  */ "script-interface text-spanner-interface dots-interface note-head-interface",
		/* reads */ "",
		/* write */ "");
