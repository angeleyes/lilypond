/*   
  measure-grouping-engraver.cc --  implement Measure_grouping_engraver

  source file of the GNU LilyPond music typesetter

  (c) 2002--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
 */

#include "warn.hh"
#include "side-position-interface.hh"
#include "global-context.hh"
#include "engraver.hh"

class Measure_grouping_engraver : public Engraver
{
public:
  TRANSLATOR_DECLARATIONS (Measure_grouping_engraver);

protected:
  Spanner * grouping_;
  Rational stop_grouping_mom_;

  virtual void process_music ();
  virtual void finalize ();
  virtual void acknowledge_grob (Grob_info);
};

void
Measure_grouping_engraver::finalize ()
{
  if (grouping_)
    {
      grouping_->set_bound (RIGHT, unsmob_grob (get_property ("currentCommandColumn")));
      grouping_->suicide ();
      grouping_ = 0;
    }
}


void
Measure_grouping_engraver::acknowledge_grob (Grob_info gi)
{
  if (grouping_)
    {
      Side_position_interface::add_support (grouping_, gi.grob_);
    }
}

void
Measure_grouping_engraver::process_music ()
{
  Moment now = now_mom ();
  if (grouping_ && now.main_part_ >= stop_grouping_mom_ && !now.grace_part_)
    {
      grouping_->set_bound (RIGHT,
			    unsmob_grob (get_property ("currentMusicalColumn")));
      
      grouping_ = 0;
    }
  
  if (now.grace_part_)
    return; 
  
  SCM grouping = get_property ("beatGrouping");
  if (scm_is_pair (grouping))
    {
      Moment *measpos = unsmob_moment (get_property ("measurePosition"));
      Rational mp = measpos->main_part_;
      
      Moment * beatlen = unsmob_moment (get_property ("beatLength"));
      Rational bl = beatlen->main_part_;
	
      Rational where (0);
      for (SCM s = grouping; scm_is_pair (s);
	   where += Rational ((int) scm_to_int (scm_car (s))) * bl,
	   s = scm_cdr (s)
	   )
	{
	  int grouplen = scm_to_int (scm_car (s));
	  if (where == mp)
	    {
	      if (grouping_)
		{
		  programming_error ("Huh, last grouping not finished yet.");
		  continue;
		}
	      
	      grouping_ = make_spanner ("MeasureGrouping", SCM_EOL);
	      grouping_->set_bound (LEFT, unsmob_grob (get_property ("currentMusicalColumn")));
	      


	      stop_grouping_mom_ = now.main_part_ + Rational (grouplen - 1) * bl ;
	      get_global_context ()->add_moment_to_process (Moment (stop_grouping_mom_));

	      if (grouplen == 3)
		grouping_->set_property ("style", ly_symbol2scm ("triangle"));
	      else
		grouping_->set_property ("style", ly_symbol2scm ("bracket"));
	      
	      break ; 
	    }
	}
    }
}
Measure_grouping_engraver::Measure_grouping_engraver ()
{
  grouping_ = 0;
}

ENTER_DESCRIPTION (Measure_grouping_engraver,
/* descr */       "Creates MeasureGrouping to indicate beat subdivision.",
/* creats*/       "MeasureGrouping",
/* accepts */     "",
/* acks  */      "note-column-interface",
/* reads */       "beatGrouping beatLength measurePosition currentMusicalColumn",
/* write */       "");
