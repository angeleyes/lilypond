/*   
  beam-engraver.cc --  implement Beam_engraver
  
  source file of the GNU LilyPond music typesetter
  
  (c)  1998--2003 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  
 */
#include "engraver-group-engraver.hh"
#include "engraver.hh"
#include "event.hh"
#include "beam.hh"
#include "stem.hh"
#include "warn.hh"
#include "beaming.hh"
#include "score-engraver.hh"
#include "rest.hh"
#include "drul-array.hh"
#include "item.hh"
#include "spanner.hh"

class Beam_engraver : public Engraver
{
protected:  
  Drul_array<Music*> evs_drul_;
  
  Spanner *finished_beam_;
  Spanner *beam_;
  Music * prev_start_ev_;

  Music * now_stop_ev_;
  
  Beaming_info_list * beam_info_;
  Beaming_info_list * finished_beam_info_;  

  /// location  within measure where beam started.
  Moment beam_start_location_;

  /// moment (global time) where beam started.
  Moment beam_start_mom_;

  bool subdivide_beams_;
  Moment beat_length_;

  void typeset_beam ();
  void set_melisma (bool);

  Moment last_stem_added_at_;
    virtual void stop_translation_timestep ();
  virtual void start_translation_timestep ();
  virtual void finalize ();

  virtual void acknowledge_grob (Grob_info);
  virtual bool try_music (Music*);
  virtual void process_music ();

  virtual bool valid_start_point();
  virtual bool valid_end_point ();
  
public:
  TRANSLATOR_DECLARATIONS(  Beam_engraver );
};


/*
  Hmm. this isn't necessary, since grace beams and normal beams are
  always nested.
 */
bool
Beam_engraver::valid_start_point()
{
  Moment n = now_mom ();

  return n.grace_part_ == Rational (0);
}

bool
Beam_engraver::valid_end_point()
{
  return valid_start_point ();
}

Beam_engraver::Beam_engraver ()
{
  beam_ = 0;
  finished_beam_ =0;
  finished_beam_info_=0;
  beam_info_ =0;
  now_stop_ev_ = 0;
  evs_drul_[LEFT] = evs_drul_[RIGHT] =0;
  prev_start_ev_ =0;
}

bool
Beam_engraver::try_music (Music *m)
{
  if (m->is_mus_type ("abort-event"))
    {
      evs_drul_[START] = 0;
      evs_drul_[STOP] = 0;
      if (beam_)
	beam_->suicide ();
      beam_ = 0;
    }
  else if (m->is_mus_type ("beam-event"))
    {
      Direction d = to_dir (m->get_mus_property ("span-direction"));
      if (d == START && !valid_start_point ())
	return false;
      if (d == STOP && !valid_end_point ())
	return false;

      if (d == START)
	{
	  evs_drul_[d] = m;
	}
      else if (d==STOP)
	{
	  now_stop_ev_ = m;
	}
      return true;
    }
  return false;
}

void
Beam_engraver::set_melisma (bool ml)
{
  SCM b = get_property ("autoBeaming");
  if (!to_boolean (b))
    daddy_trans_->set_property ("beamMelismaBusy", ml ? SCM_BOOL_T :SCM_BOOL_F);
}

void
Beam_engraver::process_music ()
{
  if (evs_drul_[STOP])
    {
      prev_start_ev_ =0;
      finished_beam_ = beam_;
      finished_beam_info_ = beam_info_;

      beam_info_ =0;
      beam_ = 0;
    }

  if (beam_ && !to_boolean (get_property ("allowBeamBreak")))
    {
      top_engraver ()->forbid_breaks ();
    }

  if (evs_drul_[START])
    {
      if (beam_)
	{
	  evs_drul_[START]->origin ()->warning (_ ("already have a beam"));
	  return;
	}

      set_melisma (true);
      prev_start_ev_ = evs_drul_[START];
      beam_ = new Spanner (get_property ("Beam"));
      SCM smp = get_property ("measurePosition");
      Moment mp = (unsmob_moment (smp)) ? *unsmob_moment (smp) : Moment (0);

      beam_start_location_ = mp;
      beam_start_mom_ = now_mom ();
      
      beam_info_ = new Beaming_info_list;
      
      /* urg, must copy to Auto_beam_engraver too */
 
      announce_grob(beam_, evs_drul_[START]->self_scm());
    }

}


void
Beam_engraver::typeset_beam ()
{
  if (finished_beam_)
    {
      finished_beam_info_->beamify(beat_length_, subdivide_beams_);
      Beam::set_beaming (finished_beam_, finished_beam_info_);
      typeset_grob (finished_beam_);
      delete finished_beam_info_;
      finished_beam_info_ =0;
      finished_beam_ = 0;
    }
}

void
Beam_engraver::start_translation_timestep ()
{
  evs_drul_ [START] =0;
  evs_drul_[STOP] = 0;
  
  if (beam_)
    {
      set_melisma (true);
      
      subdivide_beams_ = to_boolean(get_property("subdivideBeams"));
      beat_length_ = *unsmob_moment (get_property ("beatLength"));
    }
}

void
Beam_engraver::stop_translation_timestep ()
{
  typeset_beam ();
  if (now_stop_ev_ )
    {
      finished_beam_ = beam_;
      finished_beam_info_ = beam_info_;

      now_stop_ev_ = 0;
      beam_ = 0;
      beam_info_ = 0;
      typeset_beam();
	  set_melisma (false);
    }
}

void
Beam_engraver::finalize ()
{
  typeset_beam ();
  if (beam_)
    {
      prev_start_ev_->origin ()->warning (_ ("unterminated beam"));

      /*
	we don't typeset it, (we used to, but it was commented
	out. Reason unknown) */
      beam_->suicide ();
      delete beam_info_;
    }
}

void
Beam_engraver::acknowledge_grob (Grob_info info)
{
  if (beam_)
    {
      if (Rest::has_interface (info.grob_))
	{
	  info.grob_->add_offset_callback (Beam::rest_collision_callback_proc, Y_AXIS);
	}
      else if (Stem::has_interface (info.grob_))
	{
	  Moment now = now_mom();

	  if (!valid_start_point ())
	    return ;
	  
	  Item *stem = dynamic_cast<Item*> (info.grob_);
	  if (Stem::get_beam (stem))
	    return;

	  Music* m = info.music_cause();
	  if (!m->is_mus_type ("rhythmic-event"))
	    {
	      String s = _ ("stem must have Rhythmic structure");
	      if (info.music_cause ())
		info.music_cause ()->origin ()->warning (s);
	      else
		::warning (s);
	  
	      return;
	    }


	  last_stem_added_at_ = now;
	  int durlog  = unsmob_duration (m->get_mus_property ("duration"))-> duration_log ();
	  if (durlog <= 2)
	    {
	      m->origin ()->warning (_ ("stem doesn't fit in beam"));
	      prev_start_ev_->origin ()->warning (_ ("beam was started here"));
	      /*
		don't return, since

		[r4 c8] can just as well be modern notation.
	      */
	    }

	  stem->set_grob_property ("duration-log",
				    scm_int2num (durlog));
	  Moment stem_location = now - beam_start_mom_ + beam_start_location_;
	  beam_info_->add_stem (stem_location,
 (durlog- 2) >? 0);
	  Beam::add_stem (beam_, stem);
	}
    }
}





ENTER_DESCRIPTION(Beam_engraver,
/* descr */       "Handles Beam events by engraving Beams.    If omitted, then notes will be "
"printed with flags instead of beams.",
/* creats*/       "Beam",
/* accepts */     "beam-event abort-event",
/* acks  */      "stem-interface rest-interface",
/* reads */       "beamMelismaBusy beatLength subdivideBeams",
/* write */       "");


class Grace_beam_engraver : public Beam_engraver
{
public:
  TRANSLATOR_DECLARATIONS(Grace_beam_engraver);  

protected:
  virtual bool valid_start_point();
  virtual bool valid_end_point ();
};

Grace_beam_engraver::Grace_beam_engraver()
{
}

bool
Grace_beam_engraver::valid_start_point()
{
  Moment n = now_mom ();

  return n.grace_part_ != Rational (0);
}


bool
Grace_beam_engraver::valid_end_point ()
{
  return beam_ && valid_start_point ();
}



ENTER_DESCRIPTION(Grace_beam_engraver,
/* descr */       "Handles Beam events by engraving Beams.  If omitted, then notes will "
"be printed with flags instead of beams. Only engraves beams when we "
" are at grace points in time. "
,
/* creats*/       "Beam",
/* accepts */     "beam-event abort-event",
/* acks  */      "stem-interface rest-interface",
/* reads */       "beamMelismaBusy beatLength allowBeamBreak subdivideBeams",
/* write */       "");

