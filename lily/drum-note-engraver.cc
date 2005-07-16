/*
  drum-note-engraver.cc

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include <cctype>

#include "rhythmic-head.hh"
#include "engraver.hh"
#include "warn.hh"
#include "side-position-interface.hh"
#include "script-interface.hh"
#include "stem.hh"
#include "note-column.hh"
#include "duration.hh"

class Drum_notes_engraver : public Engraver
{
  Link_array<Item> notes_;
  Link_array<Item> dots_;
  Link_array<Item> scripts_;
  Link_array<Music> events_;

public:
  TRANSLATOR_DECLARATIONS (Drum_notes_engraver);

protected:
  virtual bool try_music (Music *ev);
  virtual void process_music ();
  virtual void acknowledge_grob (Grob_info);
  virtual void stop_translation_timestep ();
};

Drum_notes_engraver::Drum_notes_engraver ()
{
}

bool
Drum_notes_engraver::try_music (Music *m)
{
  if (m->is_mus_type ("note-event"))
    {
      events_.push (m);
      return true;
    }
  else if (m->is_mus_type ("busy-playing-event"))
    return events_.size ();

  return false;
}

void
Drum_notes_engraver::process_music ()
{
  SCM tab = 0;
  for (int i = 0; i < events_.size (); i++)
    {
      if (!tab)
	tab = get_property ("drumStyleTable");

      Music *ev = events_[i];
      Item *note = make_item ("NoteHead", ev->self_scm ());

      Duration dur = *unsmob_duration (ev->get_property ("duration"));

      note->set_property ("duration-log", scm_int2num (dur.duration_log ()));

      if (dur.dot_count ())
	{
	  Item *d = make_item ("Dots", ev->self_scm ());
	  Rhythmic_head::set_dots (note, d);

	  if (dur.dot_count ()
	      != robust_scm2int (d->get_property ("dot-count"), 0))
	    d->set_property ("dot-count", scm_int2num (dur.dot_count ()));

	  d->set_parent (note, Y_AXIS);

	  dots_.push (d);
	}

      SCM drum_type = ev->get_property ("drum-type");

      SCM defn = SCM_EOL;

      if (scm_hash_table_p (tab) == SCM_BOOL_T)
	defn = scm_hashq_ref (tab, drum_type, SCM_EOL);

      if (scm_is_pair (defn))
	{
	  SCM pos = scm_caddr (defn);
	  SCM style = scm_car (defn);
	  SCM script = scm_cadr (defn);

	  if (scm_integer_p (pos) == SCM_BOOL_T)
	    note->set_property ("staff-position", pos);
	  if (scm_is_symbol (style))
	    note->set_property ("style", style);

	  if (scm_is_string (script))
	    {
	      Item *p = make_item ("Script", ev->self_scm ());
	      bool follow;
	      make_script_from_event (p, &follow,
				      context (), script,
				      0);

	      if (p->get_property ("follow-into-staff"))
		p->set_property ("staff-padding", SCM_EOL);

	      p->set_parent (note, Y_AXIS);
	      Side_position_interface::add_support (p, note);
	      scripts_.push (p);
	    }
	}

      notes_.push (note);
    }
}

void
Drum_notes_engraver::acknowledge_grob (Grob_info inf)
{
  if (Stem::has_interface (inf.grob ()))
    {
      for (int i = 0; i < scripts_.size (); i++)
	{
	  Grob *e = scripts_[i];

	  if (to_dir (e->get_property ("side-relative-direction")))
	    e->set_object ("direction-source", inf.grob ()->self_scm ());

	  /*
	    add dep ?
	  */
	  e->add_dependency (inf.grob ());
	  Side_position_interface::add_support (e, inf.grob ());
	}
    }
  else if (Note_column::has_interface (inf.grob ()))
    {
      for (int i = 0; i < scripts_.size (); i++)
	{
	  Grob *e = scripts_[i];

	  if (!e->get_parent (X_AXIS)
	      && Side_position_interface::get_axis (e) == Y_AXIS)
	    {
	      e->set_parent (inf.grob (), X_AXIS);
	    }
	}
    }
}

void
Drum_notes_engraver::stop_translation_timestep ()
{
  notes_.clear ();
  dots_.clear ();
  scripts_.clear ();

  events_.clear ();
}

ADD_TRANSLATOR (Drum_notes_engraver,
		/* descr */ "Generate noteheads.",
		/* creats*/ "NoteHead Dots Script",
		/* accepts */ "note-event busy-playing-event",
		/* acks  */ "stem-interface note-column-interface",
		/* reads */ "drumStyleTable",
		/* write */ "");

