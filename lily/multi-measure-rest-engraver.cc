/*
  multi_measure_rest-engraver.cc -- implement Multi_measure_rest_engraver

  (c) 1998--2003 Jan Nieuwenhuizen <janneke@gnu.org>
       Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "event.hh"
#include "multi-measure-rest.hh"
#include "paper-column.hh"
#include "engraver-group-engraver.hh"
#include "side-position-interface.hh"
#include "staff-symbol-referencer.hh"
#include "engraver.hh"
#include "moment.hh"
#include "spanner.hh"

/**
   The name says it all: make multi measure rests 

*/
class Multi_measure_rest_engraver : public Engraver
{
public:
  TRANSLATOR_DECLARATIONS(Multi_measure_rest_engraver);

protected:
  virtual bool try_music (Music*);
  virtual void process_music ();
  virtual void stop_translation_timestep ();
  virtual void start_translation_timestep ();
  virtual void finalize ();

private:
  Music * rest_ev_;
  Link_array<Music> text_events_;
  int start_measure_;
  Rational last_main_moment_;
  Moment stop_moment_;
  
  bool bar_seen_;
  
  Spanner *last_rest_;
  Spanner *mmrest_;

  Link_array<Spanner> numbers_;
  Link_array<Spanner> last_numbers_;
};

Multi_measure_rest_engraver::Multi_measure_rest_engraver ()
{
  bar_seen_ = false;
  start_measure_ = 0;
  mmrest_ = 0;
  last_rest_ =0;
  rest_ev_ = 0;
}

bool
Multi_measure_rest_engraver::try_music (Music* req)
{
  if (req->is_mus_type ("multi-measure-rest-event"))
    {
      rest_ev_ = req;
      stop_moment_ = now_mom () + rest_ev_->get_length ();
      
      return true;
    }
  else if (req->is_mus_type ("multi-measure-text-event"))
    {
      text_events_.push (req);
      return true;
    }
  return false;
}

void
Multi_measure_rest_engraver::process_music ()
{
  if (rest_ev_ && !mmrest_
      && stop_moment_ > now_mom ())
    {
      mmrest_ = make_spanner ("MultiMeasureRest");

      if (text_events_.size())
	{
	  for (int i = 0; i < text_events_.size(); i++)
	    {
	      Spanner *sp
		= make_spanner ("MultiMeasureRestText");

	      Music* e = text_events_[i];
	      SCM t = e->get_mus_property ("text");
	      SCM dir = e->get_mus_property ("direction");
	      sp->set_grob_property ("text",t);
	      if (is_direction (dir))
		sp->set_grob_property ("direction",dir);
	      
	      numbers_.push (sp);
	      announce_grob (sp, e->self_scm());
	    }

	  /*
	    Stack different scripts.
	   */
	  Direction d = DOWN; 
	  do {
	    Grob *last =0;
	    for (int i=0; i <numbers_.size(); i++)
	      {
		if (gh_int2scm (d) == numbers_[i]->get_grob_property ("direction"))
		  {
		    if (last)
		      Side_position_interface::add_support (numbers_[i], last);
		    last = numbers_[i];
		  }
	      }
	  } while (flip (&d) != DOWN);
	}
      else
	{
	  Spanner *sp
	    = make_spanner ("MultiMeasureRestNumber");
	  numbers_.push (sp);
	  announce_grob (sp, rest_ev_->self_scm());
	}

      for (int i =0 ; i < numbers_.size(); i++)
	{
	  Side_position_interface::add_support (numbers_[i], mmrest_);
	  numbers_[i]->set_parent (mmrest_, Y_AXIS);
	}
      
      announce_grob (mmrest_, rest_ev_->self_scm());
      start_measure_
	= gh_scm2int (get_property ("currentBarNumber"));
    }

  bar_seen_ = gh_string_p (get_property ("whichBar"));
}

void
Multi_measure_rest_engraver::stop_translation_timestep ()
{
  /*
    We can not do this earlier, as breakableSeparationItem is not yet there.
  */
  
  if (bar_seen_)
    {
      Grob *cmc = unsmob_grob (get_property("breakableSeparationItem"));
      if (!cmc)
	cmc = unsmob_grob (get_property ("currentCommandColumn"));
      
      Item *it = dynamic_cast<Item*> (cmc);
      
      if (mmrest_)
	{
	  add_bound_item (mmrest_, it);
	  for (int i = 0; i < numbers_.size(); i++)
	    add_bound_item (numbers_[i], it);
	}
      
      if (last_rest_)
	{
	  add_bound_item (last_rest_,it);
	  for (int i = 0; i < last_numbers_.size(); i++)
	    add_bound_item (last_numbers_[i], it);
	}      
    }
  
  
  SCM smp = get_property ("measurePosition");
  Moment mp = (unsmob_moment (smp)) ? *unsmob_moment (smp) : Moment (0);

  if (mmrest_
      && !mp.to_bool ()
      && mmrest_->get_bound (LEFT) && mmrest_->get_bound (RIGHT))
    {
      typeset_grob (mmrest_);
      for (int i = 0 ; i < numbers_.size(); i++)
	{
	  typeset_grob (numbers_[i]);
	}
      
      /*
	we must keep mmrest_ around to set measure-count, so
	no mmrest_ = 0 here. 
       */
    }

  if (last_rest_)
    {
      /* sanity check */
      if (last_rest_->get_bound (LEFT) && last_rest_->get_bound (RIGHT)
	  && last_rest_->get_bound (LEFT) != last_rest_->get_bound (RIGHT))
	{
	  typeset_grob (last_rest_);

	  for (int i = 0; i < last_numbers_.size ();i++)
	    {
	      typeset_grob (last_numbers_[i]);
	    }
	}
      last_rest_ = 0;
      last_numbers_.clear();
    }

  text_events_.clear ();
}

void
Multi_measure_rest_engraver::start_translation_timestep ()
{
  if (now_mom ().main_part_ >= stop_moment_.main_part_)
    {
      rest_ev_ = 0;
    }

  bar_seen_ = false;

  SCM smp = get_property ("measurePosition");
  Moment mp = (unsmob_moment (smp)) ? *unsmob_moment (smp) : Moment (0);

  Moment now =now_mom ();
  if (mmrest_
      && now.main_part_ != last_main_moment_
      && mp.main_part_ == Rational (0))
    {
      last_rest_ = mmrest_;
      last_numbers_ = numbers_;
      
      int cur = gh_scm2int (get_property ("currentBarNumber"));
      int num = cur - start_measure_;
      last_rest_->set_grob_property ("measure-count", gh_int2scm (num));

      SCM sml = get_property ("measureLength");
      Rational ml = (unsmob_moment (sml)) ? unsmob_moment (sml)->main_part_ : Rational (1);
      if (ml >= Rational (2))
	{
	  last_rest_->set_grob_property ("use-breve-rest", SCM_BOOL_T);
	}

      mmrest_ = 0;
      numbers_.clear ();
      
      Grob * last = last_numbers_.size() ? last_numbers_[0] : 0;
      if (last && last->get_grob_property ("text") == SCM_EOL)
	{
	  SCM thres = get_property ("restNumberThreshold");
	  int t = 1;
	  if (gh_number_p (thres))
	    t = gh_scm2int (thres);
      
	  if (num <= t)
	    last->suicide();
	  else 
	    {
	      SCM text
		= scm_number_to_string (gh_int2scm (num), SCM_MAKINUM (10));
	      last->set_grob_property ("text", text);
	    }
	}
    }

  last_main_moment_ = now.main_part_;
}

void
Multi_measure_rest_engraver::finalize ()
{
  if (mmrest_)
    {
      typeset_grob (mmrest_);
    }
  if (last_rest_)
    {
      typeset_grob (last_rest_);
    }

  for (int i = 0; i < last_numbers_.size ();i++)
    typeset_grob (last_numbers_[i]);

  for (int i = 0; i < numbers_.size ();i++)
    typeset_grob (numbers_[i]);

}

ENTER_DESCRIPTION(Multi_measure_rest_engraver,
/* descr */
		  "Engraves multi-measure rests that are produced with @code{R}.  Reads "
"measurePosition and currentBarNumber to determine what number to print "
"over the MultiMeasureRest.  Reads measureLength to determine if it "
"should use a whole rest or a breve rest to represent 1 measure "
		  ,
/* creats*/       "MultiMeasureRest MultiMeasureRestNumber MultiMeasureRestText",
/* accepts */     "multi-measure-rest-event multi-measure-text-event",
/* acks  */      "",
/* reads */       "currentBarNumber restNumberThreshold breakableSeparationItem currentCommandColumn measurePosition measureLength",
/* write */       "");
