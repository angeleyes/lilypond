/*
  completion-note-heads-engraver.cc -- Completion_heads_engraver

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include <cctype>

#include "rhythmic-head.hh"
#include "output-def.hh"
#include "music.hh"
#include "dots.hh"
#include "dot-column.hh"
#include "staff-symbol-referencer.hh"
#include "item.hh"
#include "score-engraver.hh"
#include "warn.hh"
#include "spanner.hh"
#include "tie.hh"
#include "global-context.hh"
#include "duration.hh"
#include "pitch.hh"



/*
  TODO: make matching rest engraver.
*/

/*
  How does this work?

  When we catch the note, we predict the end of the note. We keep the
  events living until we reach the predicted end-time.

  Every time process_music () is called and there are note events, we
  figure out how long the note to typeset should be. It should be no
  longer than what's specified, than what is left to do and it should
  not cross barlines.

  We copy the events into scratch note events, to make sure that we get
  all durations exactly right.
*/

class Completion_heads_engraver : public Engraver
{
  Link_array<Item> notes_;
  Link_array<Item> prev_notes_;
  Link_array<Grob> ties_;

  Link_array<Item> dots_;
  Link_array<Music> note_events_;
  Link_array<Music> scratch_note_events_;

  Moment note_end_mom_;
  bool is_first_;
  Rational left_to_do_;
  Rational do_nothing_until_;

  Moment next_barline_moment ();
  Duration find_nearest_duration (Rational length);

public:
  TRANSLATOR_DECLARATIONS (Completion_heads_engraver);

protected:
  virtual void initialize ();
  PRECOMPUTED_VIRTUAL void start_translation_timestep ();
  virtual bool try_music (Music *event);
  PRECOMPUTED_VIRTUAL void process_music ();
  PRECOMPUTED_VIRTUAL void stop_translation_timestep ();
};

void
Completion_heads_engraver::initialize ()
{
  is_first_ = false;
}

bool
Completion_heads_engraver::try_music (Music *m)
{
  if (m->is_mus_type ("note-event"))
    {
      note_events_.push (m);

      is_first_ = true;
      Moment musiclen = m->get_length ();
      Moment now = now_mom ();

      if (now_mom ().grace_part_)
	{
	  musiclen.grace_part_ = musiclen.main_part_;
	  musiclen.main_part_ = Rational (0, 1);
	}
      note_end_mom_ = max (note_end_mom_, (now + musiclen));
      do_nothing_until_ = Rational (0, 0);

      return true;
    }
  else if (m->is_mus_type ("busy-playing-event"))
    {
      return note_events_.size () && is_first_;
    }

  return false;
}

/*
  The duration _until_ the next barline.
*/
Moment
Completion_heads_engraver::next_barline_moment ()
{
  Moment *e = unsmob_moment (get_property ("measurePosition"));
  Moment *l = unsmob_moment (get_property ("measureLength"));
  if (!e || !l)
    {
      programming_error ("no timing props set?");
      return Moment (1, 1);
    }

  return (*l - *e);
}

Duration
Completion_heads_engraver::find_nearest_duration (Rational length)
{
  int log_limit = 6;

  Duration d (0, 0);

  /*
    this could surely be done more efficient. Left to the reader as an
    excercise.  */
  while (d.get_length () > length && d.duration_log () < log_limit)
    {
      if (d.dot_count ())
	{
	  d = Duration (d.duration_log (), d.dot_count ()- 1);
	  continue;
	}
      else
	{
	  d = Duration (d.duration_log () + 1, 2);
	}
    }

  if (d.duration_log () >= log_limit)
    {
      // junk the dots.
      d = Duration (d.duration_log (), 0);

      // scale up.
      d = d.compressed (length / d.get_length ());
    }

  return d;
}

void
Completion_heads_engraver::process_music ()
{
  if (!is_first_ && !left_to_do_)
    return;

  is_first_ = false;

  Moment now = now_mom ();
  if (do_nothing_until_ > now.main_part_)
    return;

  Duration note_dur;
  Duration *orig = 0;
  if (left_to_do_)
    {
      note_dur = find_nearest_duration (left_to_do_);
    }
  else
    {
      orig = unsmob_duration (note_events_[0]->get_property ("duration"));
      note_dur = *orig;
    }
  Moment nb = next_barline_moment ();
  if (nb < note_dur.get_length ())
    {
      note_dur = find_nearest_duration (nb.main_part_);

      Moment next = now;
      next.main_part_ += note_dur.get_length ();

      get_global_context ()->add_moment_to_process (next);
      do_nothing_until_ = next.main_part_;
    }

  if (orig)
    {
      left_to_do_ = orig->get_length ();
    }

  if (orig && note_dur.get_length () != orig->get_length ())
    {
      if (!scratch_note_events_.size ())
	for (int i = 0; i < note_events_.size (); i++)
	  {
	    Music *m = note_events_[i]->clone ();
	    scratch_note_events_.push (m);
	  }
    }

  for (int i = 0;
       left_to_do_ && i < note_events_.size (); i++)
    {
      Music *event = note_events_[i];
      if (scratch_note_events_.size ())
	{
	  event = scratch_note_events_[i];
	  SCM pits = note_events_[i]->get_property ("pitch");
	  event->set_property ("pitch", pits);
	}

      event->set_property ("duration", note_dur.smobbed_copy ());

      Item *note = make_item ("NoteHead", event->self_scm ());
      note->set_property ("duration-log",
			  scm_int2num (note_dur.duration_log ()));

      int dots = note_dur.dot_count ();
      if (dots)
	{
	  Item *d = make_item ("Dots", SCM_EOL);
	  Rhythmic_head::set_dots (note, d);

	  /*
	    measly attempt to save an eeny-weenie bit of memory.
	  */
	  if (dots != scm_to_int (d->get_property ("dot-count")))
	    d->set_property ("dot-count", scm_int2num (dots));

	  d->set_parent (note, Y_AXIS);
	  dots_.push (d);
	}

      Pitch *pit = unsmob_pitch (event->get_property ("pitch"));

      int pos = pit->steps ();
      SCM c0 = get_property ("middleCPosition");
      if (scm_is_number (c0))
	pos += scm_to_int (c0);

      note->set_property ("staff-position", scm_int2num (pos));
      notes_.push (note);
    }

  if (prev_notes_.size () == notes_.size ())
    {
      for (int i = 0; i < notes_.size (); i++)
	{
	  Grob *p = make_spanner ("Tie", SCM_EOL);
	  Tie::set_interface (p); // cannot remove yet!

	  Tie::set_head (p, LEFT, prev_notes_[i]);
	  Tie::set_head (p, RIGHT, notes_[i]);

	  ties_.push (p);
	}
    }

  left_to_do_ -= note_dur.get_length ();

  /*
    don't do complicated arithmetic with grace notes.
  */
  if (orig
      && now_mom ().grace_part_)
    {
      left_to_do_ = Rational (0, 0);
    }
}

void
Completion_heads_engraver::stop_translation_timestep ()
{
  ties_.clear ();

  if (notes_.size ())
    prev_notes_ = notes_;
  notes_.clear ();

  dots_.clear ();

  for (int i = scratch_note_events_.size (); i--;)
    {
      scm_gc_unprotect_object (scratch_note_events_[i]->self_scm ());
    }

  scratch_note_events_.clear ();
}

void
Completion_heads_engraver::start_translation_timestep ()
{
  Moment now = now_mom ();
  if (note_end_mom_.main_part_ <= now.main_part_)
    {
      note_events_.clear ();
      prev_notes_.clear ();
    }
}

Completion_heads_engraver::Completion_heads_engraver ()
{
}

#include "translator.icc"

ADD_TRANSLATOR (Completion_heads_engraver,
		/* descr */ "This engraver replaces "
		"@code{Note_heads_engraver}. It plays some trickery to "
		"break long notes and automatically tie them into the next measure.",
		/* creats*/ "NoteHead Dots Tie",
		/* accepts */ "busy-playing-event note-event",
		/* reads */ "middleCPosition measurePosition measureLength",
		/* write */ "");
