/*   
  grob-pq-engraver.cc --  implement Grob_pq_engraver

  source file of the GNU LilyPond music typesetter

  (c) 2001--2004  Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "context.hh"
#include "engraver.hh"
#include "grob.hh"
#include "warn.hh"

class Grob_pq_engraver: public Engraver
{
public:
  TRANSLATOR_DECLARATIONS (Grob_pq_engraver);
protected:
  virtual void initialize ();
  virtual void acknowledge_grob (Grob_info);
  virtual void start_translation_timestep ();
  virtual void stop_translation_timestep ();
};


Grob_pq_engraver::Grob_pq_engraver ()
{
}

void
Grob_pq_engraver::initialize ()
{
  daddy_context_->set_property ("busyGrobs", SCM_EOL); 
}

LY_DEFINE (ly_grob_pq_less_p, "ly:grob-pq-less?",
	  2, 0 ,0, (SCM a, SCM b), 
	  "Compare 2 Grob PQ entries. Internal")
{
  if (Moment::compare (*unsmob_moment (gh_car (a)),
		       *unsmob_moment (gh_car (b))) < 0)
    return SCM_BOOL_T;
  else
    return SCM_BOOL_F;
}
	  
void
Grob_pq_engraver::acknowledge_grob (Grob_info gi)
{
  Music  * m = gi.music_cause ();

  if (m
      && !gi.grob_->internal_has_interface (ly_symbol2scm ("multi-measure-interface")))
    {
      Moment n = now_mom ();
      Moment l = m->get_length ();

      if (!l.to_bool ())
	return ;
      
      if (n.grace_part_)
	{
	  l.grace_part_ = l.main_part_;
	  l.main_part_ = 0;
	}

      Moment end = n + l;
      SCM lst = scm_acons (end.smobbed_copy (),
			 gi.grob_->self_scm (),
			 SCM_EOL);

      SCM busy= get_property ("busyGrobs");
      busy = scm_merge_x (lst, busy, ly_grob_pq_less_p_proc);
      daddy_context_->set_property ("busyGrobs", busy);
    }
}


void
Grob_pq_engraver::stop_translation_timestep ()
{
  Moment now = now_mom ();
  SCM start_busy = get_property ("busyGrobs");
  SCM busy = start_busy;
  while (gh_pair_p (busy) && *unsmob_moment (gh_caar (busy)) == now)
    {
      busy = gh_cdr (busy);
    }

  if (start_busy != busy)
    daddy_context_->set_property ("busyGrobs", busy);
}

void
Grob_pq_engraver::start_translation_timestep ()
{
  Moment now = now_mom ();

  SCM start_busy = get_property ("busyGrobs");
  SCM busy = start_busy;
  while (gh_pair_p (busy) && *unsmob_moment (gh_caar (busy)) < now)
    {
      /*
	Todo: do something sensible. The grob-pq-engraver is not water
	tight, and stuff like tupletSpannerDuration confuses it.
       */
      programming_error (_f ("Skipped something?\nGrob %s ended before "
			    "I expected it to end.",
			    unsmob_grob (gh_cdar (busy))->name ().to_str0()));
      
      busy = gh_cdr (busy);
    }

  if (start_busy != busy)
    daddy_context_->set_property ("busyGrobs", busy);

}


ENTER_DESCRIPTION (Grob_pq_engraver,

/* descr */       "Administrate when certain grobs (eg. note heads) stop playing; this \
engraver is a sort-of a failure, since it doesn't handle all sorts of \
borderline cases very well. \
",												  \
		  
/* creats*/       "",										  \
/* accepts */     "",										  \
/* acks  */      "grob-interface",								  \
/* reads */       "busyGrobs",									  \
/* write */       "busyGrobs");
