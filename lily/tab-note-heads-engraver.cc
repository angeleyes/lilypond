/*
  head-grav.cc -- part of GNU LilyPond

  (c) 1997--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/
#include <ctype.h>
#include <stdio.h>

#include "rhythmic-head.hh"
#include "paper-def.hh"
#include "event.hh"
#include "dots.hh"
#include "dot-column.hh"
#include "staff-symbol-referencer.hh"
#include "item.hh"
#include "score-engraver.hh"
#include "warn.hh"

/**
   make (guitar-like) tablature note
*/
class Tab_note_heads_engraver : public Engraver
{
  Link_array<Item> notes_;
  
  Link_array<Item> dots_;
  Link_array<Music> note_events_;
  Link_array<Music> tabstring_events_;
public:
  TRANSLATOR_DECLARATIONS (Tab_note_heads_engraver);

protected:
  virtual bool try_music (Music *event) ;
  virtual void process_music ();

  virtual void stop_translation_timestep ();
};


Tab_note_heads_engraver::Tab_note_heads_engraver ()
{
}

bool
Tab_note_heads_engraver::try_music (Music *m) 
{
  if (m->is_mus_type ("note-event"))
    {
      note_events_.push (m);
      return true;
    }
  else if (m->is_mus_type ("string-number-event"))
    {
      tabstring_events_.push (m);
      return true;
    }
  else if (m->is_mus_type ("busy-playing-event"))
    {
      return note_events_.size ();
    }
  
  return false;
}


void
Tab_note_heads_engraver::process_music ()
{
  int j = 0; 
  for (int i=0; i < note_events_.size (); i++)
    {
      SCM stringTunings = get_property ("stringTunings");
      int number_of_strings = ((int) gh_length (stringTunings));
      bool high_string_one = to_boolean (get_property ("highStringOne"));

      Item * note  = make_item ("TabNoteHead");
      
      Music * event = note_events_[i];

      
      Music * tabstring_event=0;

      for (SCM s =event->get_property ("articulations");
	   !tabstring_event && gh_pair_p (s); s = gh_cdr (s))
	{
	  Music * art = unsmob_music (gh_car (s));

	  if (art->is_mus_type ("string-number-event"))
	    tabstring_event = art;
	}

      if (!tabstring_event  && j < tabstring_events_.size ())
	{
	  tabstring_event = tabstring_events_[j];
	  if (j +1 <  tabstring_events_.size ())
	    j++;
	}

      int tab_string;
      bool string_found;
      if (tabstring_event)
	{
	  tab_string = gh_scm2int (tabstring_event->get_property ("string-number"));
	  string_found = true;
	}
      else
	{
	  tab_string = high_string_one ? 1 : number_of_strings;
	  string_found = false;
	}
      
      Duration dur = *unsmob_duration (event->get_property ("duration"));
      note->set_property ("duration-log",
			       gh_int2scm (dur.duration_log ()));

      if (dur.dot_count ())
	{
	  Item * d = make_item ("Dots");
	  Rhythmic_head::set_dots (note, d);
	  
	  if (dur.dot_count ()
	      != gh_scm2int (d->get_property ("dot-count")))
	    d->set_property ("dot-count", gh_int2scm (dur.dot_count ()));

	  d->set_parent (note, Y_AXIS);
	  announce_grob (d, SCM_EOL);
	  dots_.push (d);
	}
      
      
      SCM scm_pitch = event->get_property ("pitch");
      SCM proc      = get_property ("tablatureFormat");
      SCM min_fret_scm = get_property ("minimumFret");
      int min_fret = gh_number_p (min_fret_scm) ? gh_scm2int (min_fret_scm) : 0;

      while (!string_found)
	{
	  int fret = unsmob_pitch (scm_pitch)->semitone_pitch ()
	    - gh_scm2int (gh_list_ref (stringTunings,gh_int2scm (tab_string-1)));
	  if (fret<min_fret)
	    tab_string += high_string_one ? 1 : -1;
	  else
	    string_found = true;
	}

      SCM text = gh_call3 (proc, gh_int2scm (tab_string), stringTunings, scm_pitch);

      int pos = 2 * tab_string - number_of_strings - 1; // No tab-note between the string !!!
      if (to_boolean (get_property ("stringOneTopmost")))
	pos = -pos;
      
      note->set_property ("text", text);      
      
      note->set_property ("staff-position", gh_int2scm (pos));
      announce_grob (note, event->self_scm ());
      notes_.push (note);
    }
}

void
Tab_note_heads_engraver::stop_translation_timestep ()
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
  note_events_.clear ();
  tabstring_events_.clear ();
}


ENTER_DESCRIPTION (Tab_note_heads_engraver,
/* descr */       "Generate one or more tablature noteheads from Music of type NoteEvent.",
/* creats*/       "TabNoteHead Dots",
/* accepts */     "note-event string-number-event busy-playing-event",
/* acks  */      "",
/* reads */       "middleCPosition stringTunings minimumFret tablatureFormat highStringOne stringOneTopmost",
/* write */       "");

