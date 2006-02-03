/*
  multi_measure_rest-engraver.cc -- implement Multi_measure_rest_engraver

  (c) 1998--2006 Jan Nieuwenhuizen <janneke@gnu.org>
  Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "multi-measure-rest.hh"
#include "paper-column.hh"
#include "engraver-group.hh"
#include "side-position-interface.hh"
#include "staff-symbol-referencer.hh"
#include "moment.hh"

/**
   The name says it all: make multi measure rests
*/
class Multi_measure_rest_engraver : public Engraver
{
public:
  TRANSLATOR_DECLARATIONS (Multi_measure_rest_engraver);

protected:
  virtual bool try_music (Music *);
  void process_music ();
  void stop_translation_timestep ();
  void start_translation_timestep ();
  virtual void finalize ();

private:
  Music *rest_ev_;
  Link_array__Music_ text_events_;
  int start_measure_;
  Rational last_main_moment_;
  Moment stop_moment_;

  bool bar_seen_;
  Item *last_command_item_;
  Spanner *last_rest_;
  Spanner *mmrest_;

  Link_array__Spanner_ numbers_;
  Link_array__Spanner_ last_numbers_;
};

Multi_measure_rest_engraver::Multi_measure_rest_engraver ()
{
  last_command_item_ = 0;

  /*
    For the start of a score.
  */
  bar_seen_ = true;
  start_measure_ = 0;
  mmrest_ = 0;
  last_rest_ = 0;
  rest_ev_ = 0;
}

bool
Multi_measure_rest_engraver::try_music (Music *event)
{
  if (event->is_mus_type ("multi-measure-rest-event"))
    {
      rest_ev_ = event;
      stop_moment_ = now_mom () + rest_ev_->get_length ();

      return true;
    }
  else if (event->is_mus_type ("multi-measure-text-event"))
    {
      text_events_.push_back (event);
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
      mmrest_ = make_spanner ("MultiMeasureRest", rest_ev_->self_scm ());

      Spanner *sp
	= make_spanner ("MultiMeasureRestNumber", rest_ev_->self_scm ());
      numbers_.push_back (sp);

      if (text_events_.size ())
	{
	  for (vsize i = 0; i < text_events_.size (); i++)
	    {

	      Music *e = text_events_[i];
	      Spanner *sp
		= make_spanner ("MultiMeasureRestText", e->self_scm ());
	      SCM t = e->get_property ("text");
	      SCM dir = e->get_property ("direction");
	      sp->set_property ("text", t);
	      if (is_direction (dir))
		sp->set_property ("direction", dir);

	      numbers_.push_back (sp);
	    }

	  /*
	    Stack different scripts.
	  */
	  Direction d = DOWN;
	  do
	    {
	      Grob *last = 0;
	      for (vsize i = 0; i < numbers_.size (); i++)
		{
		  if (scm_from_int (d) == numbers_[i]->get_property ("direction"))
		    {
		      if (last)
			Side_position_interface::add_support (numbers_[i], last);
		      last = numbers_[i];
		    }
		}
	    }
	  while (flip (&d) != DOWN);
	}

      for (vsize i = 0; i < numbers_.size (); i++)
	{
	  Side_position_interface::add_support (numbers_[i], mmrest_);
	  numbers_[i]->set_parent (mmrest_, Y_AXIS);
	}

      start_measure_
	= scm_to_int (get_property ("currentBarNumber"));
    }

  bar_seen_ = bar_seen_ || scm_is_string (get_property ("whichBar"));
}

void
Multi_measure_rest_engraver::stop_translation_timestep ()
{
  /* We cannot do this earlier, as breakableSeparationItem is not yet
     there.  */
  if (bar_seen_)
    {
      Grob *cmc = unsmob_grob (get_property ("breakableSeparationItem"));
      if (!cmc)
	cmc = unsmob_grob (get_property ("currentCommandColumn"));

      /* Ugh, this is a kludge - need this for multi-measure-rest-grace.ly  */
      last_command_item_ = dynamic_cast<Item *> (cmc);
    }

  if (last_command_item_ && (mmrest_ || last_rest_))
    {
      if (last_rest_)
	{
	  add_bound_item (last_rest_, last_command_item_);
	  for (vsize i = 0; i < last_numbers_.size (); i++)
	    add_bound_item (last_numbers_[i], last_command_item_);
	}

      if (mmrest_)
	{
	  add_bound_item (mmrest_, last_command_item_);
	  for (vsize i = 0; i < numbers_.size (); i++)
	    add_bound_item (numbers_[i], last_command_item_);

	  last_command_item_ = 0;
	}
    }

  Moment mp (robust_scm2moment (get_property ("measurePosition"), Moment (0)));
  if (last_rest_)
    {
      last_rest_ = 0;
      last_numbers_.clear ();
    }

  text_events_.clear ();
  bar_seen_ = false;
}

void
Multi_measure_rest_engraver::start_translation_timestep ()
{
  if (now_mom ().main_part_ >= stop_moment_.main_part_)
    rest_ev_ = 0;

  Moment mp (robust_scm2moment (get_property ("measurePosition"), Moment (0)));

  Moment now = now_mom ();
  if (mmrest_
      && now.main_part_ != last_main_moment_
      && mp.main_part_ == Rational (0))
    {
      last_rest_ = mmrest_;
      last_numbers_ = numbers_;

      int cur = scm_to_int (get_property ("currentBarNumber"));
      int num = cur - start_measure_;

      /*
	We can't plug a markup directly into the grob, since the
	measure-count determines the formatting of the mmrest.
      */
      last_rest_->set_property ("measure-count", scm_from_int (num));

      SCM sml = get_property ("measureLength");
      Rational ml = (unsmob_moment (sml)) ? unsmob_moment (sml)->main_part_ : Rational (1);
      if (ml >= Rational (2))
	last_rest_->set_property ("use-breve-rest", SCM_BOOL_T);

      mmrest_ = 0;
      numbers_.clear ();

      Grob *last = last_numbers_.size () ? last_numbers_[0] : 0;
      if (last && last->get_property ("text") == SCM_EOL)
	{
	  SCM thres = get_property ("restNumberThreshold");
	  int t = 1;
	  if (scm_is_number (thres))
	    t = scm_to_int (thres);

	  if (num <= t)
	    last->suicide ();
	  else
	    {
	      SCM text
		= scm_number_to_string (scm_from_int (num), scm_from_int (10));
	      last->set_property ("text", text);
	    }
	}
    }

  last_main_moment_ = now.main_part_;
}

void
Multi_measure_rest_engraver::finalize ()
{
}

#include "translator.icc"

ADD_TRANSLATOR (Multi_measure_rest_engraver,
		/* doc */
		"Engraves multi-measure rests that are produced with @code{R}.  Reads "
		"measurePosition and currentBarNumber to determine what number to print "
		"over the MultiMeasureRest.  Reads measureLength to determine if it "
		"should use a whole rest or a breve rest to represent 1 measure ",
		/* create */ "MultiMeasureRest MultiMeasureRestNumber MultiMeasureRestText",
		/* accept */ "multi-measure-rest-event multi-measure-text-event",
		/* read */
		"currentBarNumber "
		"restNumberThreshold "
		"breakableSeparationItem "
		"currentCommandColumn "
		"measurePosition "
		"measureLength",
		/* write */ "");
