/*   
  volta-engraver.cc --  implement Volta_engraver
  
  source file of the GNU LilyPond music typesetter
  
  (c) 2000--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  
 */

#include "engraver.hh"
#include "context.hh"
#include "volta-bracket.hh"
#include "item.hh"
#include "note-column.hh"
#include "bar-line.hh"
#include "side-position-interface.hh"
#include "warn.hh"
#include "staff-symbol.hh"

/*
  Create Volta spanners, by reading repeatCommands  property, usually
  set by Unfolded_repeat_iterator.
 */
class Volta_engraver : public Engraver
{
public:
  TRANSLATOR_DECLARATIONS (Volta_engraver);
protected:

  virtual void acknowledge_grob (Grob_info);
  virtual void finalize ();
  virtual void stop_translation_timestep ();
  virtual void process_music ();
  
  Moment started_mom_;
  Spanner *volta_span_;
  Spanner *end_volta_span_;
  SCM staff_;
  SCM start_string_;

  bool staff_eligible ();
 
};

Volta_engraver::Volta_engraver ()
{
  staff_ = SCM_EOL;
  volta_span_ = 0;
  end_volta_span_ = 0;
}


/*
  TODO: this logic should be rewritten, it is buggy.

  One of the problems is that we can't determine wether or not to
  print the volta bracket during the first step, since that requires
  acknowledging the staff.
 */
bool
Volta_engraver::staff_eligible ()
{
  SCM doit =get_property ("voltaOnThisStaff");
  if (scm_is_bool (doit))
    {
      return to_boolean (doit);
    }


  if (!unsmob_grob (staff_))
    return false;

  /*
    TODO: this does weird things when you open a piece with a
    volta spanner.
  */
  SCM staffs = get_property ("stavesFound");

  /*
    only put a volta on the top staff.
	
    May be this is a bit convoluted, and we should have a single
    volta engraver in score context or somesuch.
	
  */
  if (!scm_is_pair (staffs))
    {
      programming_error ("Huh? Volta engraver can't find staffs?");
      return false;
    }
  else if (ly_car (scm_last_pair (staffs)) != staff_)
    {
      return false;
    }
  return true;
}


void
Volta_engraver::process_music ()
{
  SCM cs = get_property ("repeatCommands");

  if (!staff_eligible ())
    return ; 

    
  bool  end = false;
  start_string_ = SCM_EOL;
  while (scm_is_pair (cs))
    {
      SCM c = ly_car (cs);

      if (scm_is_pair (c)
	  && ly_car (c) == ly_symbol2scm ("volta")
	  && scm_is_pair (ly_cdr (c)))
	{
	  if (ly_cadr (c) ==  SCM_BOOL_F)
	    end = true;
	  else
	    start_string_ = ly_cadr (c);
	}
      
      cs = ly_cdr (cs);
    }

  if (volta_span_)
    {
      SCM l (get_property ("voltaSpannerDuration"));
      Moment now = now_mom ();
  
      bool early_stop = unsmob_moment (l)
	&& *unsmob_moment (l) <= now - started_mom_;
      
      end = end || early_stop;
    }

  
  if (end && !volta_span_)
    {
      warning (_ ("No volta spanner to end")); // fixme: be more verbose.
    }
  else if (end)
    {
      end_volta_span_ = volta_span_;
      volta_span_ =0;
    }

  if (volta_span_ && 
      (scm_is_string (start_string_) || scm_is_pair (start_string_)))
    {
      warning (_ ("Already have a volta spanner.  Stopping that one prematurely."));
      
      if (end_volta_span_)
	{
	  warning (_ ("Also have a stopped spanner.  Giving up."));
	  return ;
	}

      end_volta_span_ = volta_span_;
      volta_span_ = 0;
    }

  if (!volta_span_ && 
      (scm_is_string (start_string_) || scm_is_pair (start_string_)))
    {
      started_mom_ = now_mom () ;

      volta_span_ = make_spanner ("VoltaBracket", SCM_EOL);

      
      volta_span_->set_property ("text", start_string_);
    }
}


void
Volta_engraver::acknowledge_grob (Grob_info i)
{
  if (Item* item = dynamic_cast<Item*> (i.grob_))
    {
      if (Note_column::has_interface (item))
	{
	  if (volta_span_)
	    Volta_bracket_interface::add_column (volta_span_,item);
	}
      if (Bar_line::has_interface (item))
	{
	  if (volta_span_)
	    Volta_bracket_interface::add_bar (volta_span_, item);
	  if (end_volta_span_)
	    Volta_bracket_interface::add_bar (end_volta_span_ , item);
	}
    }
  else if (Staff_symbol::has_interface (i.grob_))
    {
      /*
	We only want to know about a single staff: then we add to the
	support.  */
      if (staff_ != SCM_EOL)
	staff_ = SCM_UNDEFINED;

      if (staff_ != SCM_UNDEFINED)
	staff_ = i.grob_->self_scm ();
    }
}

void
Volta_engraver::finalize ()
{
}



void 
Volta_engraver::stop_translation_timestep ()
{
  if (volta_span_ && !staff_eligible ())
    {
      /*
	THIS IS A KLUDGE.

	we need to do this here, because STAFF_ is not initialized yet
	in the 1st call of process_music ()
      */
      
      volta_span_->suicide ( );
      volta_span_ = 0;
    }
  
  if (end_volta_span_ && !end_volta_span_->get_bound (RIGHT))
    {
      Grob * cc = unsmob_grob (get_property ("currentCommandColumn"));
      Item * ci = dynamic_cast<Item*> (cc);
      end_volta_span_->set_bound (RIGHT, ci);
    }

  end_volta_span_ =0;

  if (volta_span_ && !volta_span_->get_bound (LEFT))
    {
      Grob * cc = unsmob_grob (get_property ("currentCommandColumn"));
      Item * ci = dynamic_cast<Item*> (cc);
      volta_span_->set_bound (LEFT, ci);
    }
  
}

/*
  TODO: should attach volta to paper-column if no bar is found.
 */

ENTER_DESCRIPTION (Volta_engraver,
/* descr */       "Make volta brackets.",
/* creats*/       "VoltaBracket",
/* accepts */     "",
/* acks  */       "bar-line-interface staff-symbol-interface note-column-interface",
/* reads */       "repeatCommands voltaSpannerDuration stavesFound",
/* write */       "");
