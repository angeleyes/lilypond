/*
  quote-iterator.cc -- implement Quote_iterator

  source file of the GNU LilyPond music typesetter

  (c) 2004--2005 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "context.hh"
#include "event.hh"
#include "music-sequence.hh"
#include "lily-guile.hh"
#include "music-wrapper-iterator.hh"
#include "warn.hh"

class Quote_iterator : public Music_wrapper_iterator
{
public:
  Quote_iterator ();
  Moment vector_moment (int idx) const;
  Interpretation_context_handle quote_outlet_;

  Moment start_moment_;
  Moment stop_moment_;
  SCM event_vector_;
  int event_idx_;
  int end_idx_;

  SCM transposed_musics_;

  DECLARE_SCHEME_CALLBACK (constructor, ());
  bool quote_ok () const;
  bool accept_music_type (Music *) const;
protected:
  virtual void derived_mark () const;
  virtual void construct_children ();
  virtual Moment pending_moment () const;
  virtual void process (Moment);
  virtual bool ok () const;
};

bool
Quote_iterator::accept_music_type (Music *mus) const
{
  SCM accept = get_outlet ()->get_property ("quotedEventTypes");
  for (SCM s = mus->get_property ("types");
       scm_is_pair (s); s = scm_cdr (s))
    {
      if (scm_memq (scm_car (s), accept) != SCM_BOOL_F)
	return true;
    }

  return false;
}

void
Quote_iterator::derived_mark () const
{
  scm_gc_mark (transposed_musics_);
}

Quote_iterator::Quote_iterator ()
{
  transposed_musics_ = SCM_EOL;
  event_vector_ = SCM_EOL;
  event_idx_ = 0;
  end_idx_ = 0;
}

int
binsearch_scm_vector (SCM vec, SCM key, bool (*is_less) (SCM a, SCM b))
{
  int lo = 0;
  int hi = scm_c_vector_length (vec);

  /* binary search */
  do
    {
      int cmp = (lo + hi) / 2;

      SCM when = scm_caar (scm_c_vector_ref (vec, cmp));
      bool result = (*is_less) (key, when);
      if (result)
	hi = cmp;
      else
	lo = cmp;
    }
  while (hi - lo > 1);

  return lo;
}

void
Quote_iterator::construct_children ()
{
  Music_wrapper_iterator::construct_children ();

  SCM name = get_music ()->get_property ("quoted-context-type");
  SCM id = get_music ()->get_property ("quoted-context-id");

  if (scm_is_string (id)
      && scm_is_symbol (name))
    {
      Context *cue_context = get_outlet ()->find_create_context (name,
								 ly_scm2string (id), SCM_EOL);
      quote_outlet_.set_context (cue_context);
    }
  else
    {
      quote_outlet_.set_context (get_outlet ());
    }

  event_vector_ = get_music ()->get_property ("quoted-events");

  /*
    We have to delay initting event_idx_ , since we have to
    take starting grace notes into account. Those may offset
    event_idx_.
  */
  event_idx_ = -1;
}

bool
Quote_iterator::ok () const
{
  return
    Music_wrapper_iterator::ok ()
    || quote_ok ();
}

bool
Quote_iterator::quote_ok () const
{
  return (event_idx_ >= 0
	  && scm_is_vector (event_vector_)
	  && event_idx_ <= end_idx_

	  /*
	    Don't quote the grace notes leading to an unquoted note.
	  */
	  && vector_moment (event_idx_).main_part_ < stop_moment_.main_part_);
}

Moment
Quote_iterator::pending_moment () const
{
  Rational infty;
  infty.set_infinite (1);
  Moment m (infty);

  if (Music_wrapper_iterator::ok ())
    m = m <? Music_wrapper_iterator::pending_moment ();

  /*
    In case event_idx_ < 0, we're not initted yet, and the wrapped
    music expression determines the starting moment.
  */
  if (quote_ok ())
    m = m <? vector_moment (event_idx_) - start_moment_;

  return m;
}

Moment
Quote_iterator::vector_moment (int idx) const
{
  SCM entry = scm_c_vector_ref (event_vector_, idx);
  return *unsmob_moment (scm_caar (entry));
}

void
Quote_iterator::process (Moment m)
{
  if (Music_wrapper_iterator::ok ())
    Music_wrapper_iterator::process (m);

  if (!scm_is_vector (event_vector_))
    return;

  if (event_idx_ < 0)
    {
      event_idx_ = binsearch_scm_vector (event_vector_,
					 get_outlet ()->now_mom ().smobbed_copy (),
					 &moment_less);
      start_moment_ = get_outlet ()->now_mom () - music_start_mom ();
      stop_moment_ = start_moment_ + get_music ()->get_length ();

      end_idx_ = binsearch_scm_vector (event_vector_,
				       stop_moment_.smobbed_copy (),
				       &moment_less);
    }

  m += start_moment_;
  while (event_idx_ <= end_idx_)
    {
      Moment em = vector_moment (event_idx_);
      if (em > m)
	return;

      if (em == m)
	break;

      event_idx_++;
    }

  if (quote_ok ())
    {
      SCM entry = scm_c_vector_ref (event_vector_, event_idx_);
      Pitch *quote_pitch = unsmob_pitch (scm_cdar (entry));

      /*
	The pitch that sounds like central C
      */
      Pitch *me_pitch = unsmob_pitch (get_outlet ()->get_property ("instrumentTransposition"));

      for (SCM s = scm_cdr (entry); scm_is_pair (s); s = scm_cdr (s))
	{
	  SCM ev_acc = scm_car (s);

	  Music *mus = unsmob_music (scm_car (ev_acc));
	  if (!mus)
	    programming_error ("need music in quote.");
	  else if (accept_music_type (mus))
	    {
	      if (quote_pitch || me_pitch)
		{
		  Pitch qp, mp;
		  if (quote_pitch)
		    qp = *quote_pitch;
		  if (me_pitch)
		    mp = *me_pitch;

		  Pitch diff = pitch_interval (qp, mp);

		  SCM copy = ly_music_deep_copy (mus->self_scm ());
		  mus = unsmob_music (copy);

		  transposed_musics_ = scm_cons (copy, transposed_musics_);
		  mus->transpose (diff);
		}

	      bool b = quote_outlet_.get_outlet ()->try_music (mus);
	      if (!b)
		mus->origin ()->warning (_f ("In quotation: junking event %s", mus->name ()));
	    }
	}

      event_idx_++;
    }
}

IMPLEMENT_CTOR_CALLBACK (Quote_iterator);
