/*
  grob-pq-engraver.cc -- implement Grob_pq_engraver

  source file of the GNU LilyPond music typesetter

  (c) 2001--2005  Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "context.hh"
#include "engraver.hh"
#include "grob.hh"
#include "warn.hh"

struct Grob_pq_entry
{
  Grob *grob_;
  Moment end_;
  static int compare (Grob_pq_entry const &a,
		      Grob_pq_entry const &b)
  {
    return Moment::compare (a.end_, b.end_);
  }
};

class Grob_pq_engraver : public Engraver
{
public:
  TRANSLATOR_DECLARATIONS (Grob_pq_engraver);
protected:
  virtual void initialize ();
  DECLARE_ACKNOWLEDGER(grob);
  PRECOMPUTED_VIRTUAL void start_translation_timestep ();
  PRECOMPUTED_VIRTUAL void stop_translation_timestep ();

  Array<Grob_pq_entry> started_now_;
};

Grob_pq_engraver::Grob_pq_engraver ()
{
}

void
Grob_pq_engraver::initialize ()
{
  context ()->set_property ("busyGrobs", SCM_EOL);
}

LY_DEFINE (ly_grob_pq_less_p, "ly:grob-pq-less?",
	   2, 0, 0, (SCM a, SCM b),
	   "Compare 2 grob priority queue entries. Internal")
{
  if (Moment::compare (*unsmob_moment (scm_car (a)),
		       *unsmob_moment (scm_car (b))) < 0)
    return SCM_BOOL_T;
  else
    return SCM_BOOL_F;
}

void
Grob_pq_engraver::acknowledge_grob (Grob_info gi)
{
  Music *m = gi.music_cause ();

  if (m
      && !gi.grob ()->internal_has_interface (ly_symbol2scm ("multi-measure-interface")))
    {
      Moment n = now_mom ();
      Moment l = m->get_length ();

      if (!l.to_bool ())
	return;

      if (n.grace_part_)
	{
	  l.grace_part_ = l.main_part_;
	  l.main_part_ = 0;
	}

      Moment end = n + l;

      Grob_pq_entry e;
      e.grob_ = gi.grob ();
      e.end_ = end;
      
      started_now_.push (e);
    }
}

void
Grob_pq_engraver::stop_translation_timestep ()
{
  Moment now = now_mom ();
  SCM start_busy = get_property ("busyGrobs");
  SCM busy = start_busy;
  while (scm_is_pair (busy) && *unsmob_moment (scm_caar (busy)) == now)
    {
      busy = scm_cdr (busy);
    }

  started_now_.sort (Grob_pq_entry::compare);
  SCM lst = SCM_EOL;
  SCM *tail = &lst;
  for (int i = 0; i < started_now_.size (); i++)
    {
      *tail = scm_acons (started_now_[i].end_.smobbed_copy (),
			 started_now_[i].grob_->self_scm (),
			 SCM_EOL);
      tail = SCM_CDRLOC(*tail);
    }
  
  busy = scm_merge_x (lst, busy, ly_grob_pq_less_p_proc);
  context ()->set_property ("busyGrobs", busy);

  started_now_.clear ();
}

void
Grob_pq_engraver::start_translation_timestep ()
{
  Moment now = now_mom ();

  SCM start_busy = get_property ("busyGrobs");
  SCM busy = start_busy;
  while (scm_is_pair (busy) && *unsmob_moment (scm_caar (busy)) < now)
    {
      /*
	The grob-pq-engraver is not water tight, and stuff like
	tupletSpannerDuration confuses it.
      */
      busy = scm_cdr (busy);
    }

  if (start_busy != busy)
    context ()->set_property ("busyGrobs", busy);
}

#include "translator.icc"
ADD_ACKNOWLEDGER(Grob_pq_engraver, grob);
ADD_TRANSLATOR (Grob_pq_engraver,

		/* descr */ "Administrate when certain grobs (eg. note heads) stop playing",
		/* creats*/ "",
		/* accepts */ "",
		/* reads */ "busyGrobs",
		/* write */ "busyGrobs");
