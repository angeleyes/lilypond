/*
  note-performer.cc -- implement Note_performer

  source file of the GNU LilyPond music typesetter

  (c) 1996--2006 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include "performer.hh"
#include "audio-item.hh"
#include "audio-column.hh"
#include "global-context.hh"
#include "stream-event.hh"
#include "warn.hh"

#include "translator.icc"

class Note_performer : public Performer
{
public:
  TRANSLATOR_DECLARATIONS (Note_performer);

protected:
  void stop_translation_timestep ();
  void process_music ();

  DECLARE_TRANSLATOR_LISTENER (note);
private:
  vector<Stream_event*> note_evs_;
  vector<Audio_note*> notes_;


  vector<Audio_note*> last_notes_;
  Moment last_start_;
  
};

void
Note_performer::process_music ()
{
  if (!note_evs_.size ())
    return;

  int transposing = 0;
  SCM prop = get_property ("instrumentTransposition");
  if (unsmob_pitch (prop))
    transposing = unsmob_pitch (prop)->semitone_pitch ();

  for (vsize i = 0; i < note_evs_.size (); i ++)
    {
      Stream_event *n = note_evs_[i];
      SCM pit = n->get_property ("pitch");

      if (Pitch *pitp = unsmob_pitch (pit))
	{
	  SCM articulations = n->get_property ("articulations");
	  Stream_event *tie_event = 0;
	  for (SCM s = articulations;
	       !tie_event && scm_is_pair (s);
	       s = scm_cdr (s))
	    {
	      Stream_event *ev = unsmob_stream_event (scm_car (s));
	      if (!ev)
		continue;
	  
	      if (ev->in_event_class ("tie-event"))
		tie_event = ev;
	    }

	  Moment len = get_event_length (n);
	  if (now_mom ().grace_part_)
	    {
	      len.grace_part_ = len.main_part_;
	      len.main_part_ = Rational (0);
	    }
	  
	  Audio_note *p = new Audio_note (*pitp, len, 
					  tie_event, -transposing);
	  Audio_element_info info (p, n);
	  announce_element (info);
	  notes_.push_back (p);

	  /*
	    shorten previous note.
	   */
	  if (now_mom ().grace_part_)
	    {
	      if (last_start_.grace_part_ == Rational (0))
		{
		  for (vsize i = 0; i < last_notes_.size (); i++)
		    last_notes_[i]->length_mom_ += Moment (0,
							   now_mom().grace_part_);
		}
	    }
	}
    }
}

void
Note_performer::stop_translation_timestep ()
{
  if (note_evs_.size ())
    {
      last_notes_ = notes_;
      last_start_ = now_mom ();
    }

  notes_.clear ();
  note_evs_.clear ();
}

IMPLEMENT_TRANSLATOR_LISTENER (Note_performer, note)
void
Note_performer::listen_note (Stream_event *ev)
{
  note_evs_.push_back (ev);
}

ADD_TRANSLATOR (Note_performer, "", "",
		"", "");

Note_performer::Note_performer ()
{
}
