/*
  note-heads-engraver.cc -- part of GNU LilyPond

  (c)  1997--2003 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/
#include <ctype.h>

#include "rhythmic-head.hh"
#include "paper-def.hh"
#include "event.hh"
#include "dots.hh"
#include "dot-column.hh"
#include "staff-symbol-referencer.hh"
#include "item.hh"
#include "engraver.hh"
#include "warn.hh"

class Note_heads_engraver : public Engraver
{
  Link_array<Item> notes_;
  Link_array<Item> dots_;
  Link_array<Music> note_reqs_;

public:
  TRANSLATOR_DECLARATIONS(Note_heads_engraver);

protected:
  virtual bool try_music (Music *req) ;
  virtual void process_music ();

  virtual void stop_translation_timestep ();
};

Note_heads_engraver::Note_heads_engraver()
{
}

bool
Note_heads_engraver::try_music (Music *m) 
{
  if (m->is_mus_type ("note-event"))
    {
      note_reqs_.push (m);
      return true;
    }
  else if (m->is_mus_type ("busy-playing-event"))
    return note_reqs_.size ();
  else if (m->is_mus_type ("start-playing-event"))
    return note_reqs_.size ();
  
  return false;
}


void
Note_heads_engraver::process_music ()
{
  for (int i=0; i < note_reqs_.size (); i++)
    {
      Item *note = make_item ("NoteHead");

      Music * req = note_reqs_[i];
      
      Duration dur = *unsmob_duration (req->get_mus_property ("duration"));

      note->set_grob_property ("duration-log", gh_int2scm (dur.duration_log ()));

      if (dur.dot_count ())
	{
	  Item * d = make_item ("Dots");
	  Rhythmic_head::set_dots (note, d);
	  
	  if (dur.dot_count ()
	      != robust_scm2int (d->get_grob_property ("dot-count"), 0))
	    d->set_grob_property ("dot-count", gh_int2scm (dur.dot_count ()));

	  d->set_parent (note, Y_AXIS);
	  announce_grob (d, SCM_EOL);
	  dots_.push (d);
	}

      Pitch *pit =unsmob_pitch (req->get_mus_property ("pitch"));

      int pos = pit ? pit->steps () : 0;
      SCM c0 = get_property ("centralCPosition");
      if (gh_number_p (c0))
	pos += gh_scm2int (c0);

      note->set_grob_property ("staff-position",   gh_int2scm (pos));
      announce_grob (note,req->self_scm());
      notes_.push (note);
    }
}

void
Note_heads_engraver::stop_translation_timestep ()
{
  for (int i=0; i < notes_.size (); i++)
    {
      typeset_grob (notes_[i]);
    }

  notes_.clear ();
  for (int i=0; i < dots_.size (); i++)
    {
      typeset_grob (dots_[i]);
    }
  dots_.clear ();
  note_reqs_.clear ();
}



ENTER_DESCRIPTION(Note_heads_engraver,
/* descr */       "Generate noteheads.",
/* creats*/       "NoteHead Dots",
/* accepts */     "note-event busy-playing-event",
/* acks  */      "",
/* reads */       "centralCPosition",
/* write */       "");
