/*   
  measure-grouping-engraver.cc --  implement Measure_grouping_engraver

  source file of the GNU LilyPond music typesetter

  (c) 2002 Han-Wen Nienhuys <hanwen@cs.uu.nl>
 */

#include "score-engraver.hh"
#include "spanner.hh"
#include "warn.hh"
#include "side-position-interface.hh"

class Measure_grouping_engraver : public Engraver
{
public:
  TRANSLATOR_DECLARATIONS(Measure_grouping_engraver);

protected:
  Spanner * grouping_;
  Rational stop_grouping_mom_;

  virtual void process_music ();
  virtual void finalize ();
  virtual void acknowledge_grob (Grob_info);
};

void
Measure_grouping_engraver::finalize()
{
  if (grouping_)
    {
      grouping_->set_bound (RIGHT, unsmob_grob (get_property ("currentCommandColumn")));
      typeset_grob (grouping_);
      grouping_->suicide ();
      grouping_= 0;
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
  Moment now = now_mom();
  if (grouping_ && now.main_part_ >= stop_grouping_mom_ && !now.grace_part_)
    {
      grouping_->set_bound (RIGHT,
			    unsmob_grob (get_property ("currentMusicalColumn")));
      typeset_grob (grouping_);
      grouping_ = 0;
    }
  
  if (now.grace_part_)
    return; 
  
  SCM grouping = get_property ("beatGrouping");
  if (gh_pair_p (grouping))
    {
      Moment *measpos = unsmob_moment (get_property ("measurePosition"));
      Rational mp = measpos->main_part_;
      
      Moment * beatlen = unsmob_moment (get_property ("beatLength"));
      Rational bl = beatlen->main_part_;
	
      Rational where (0);
      for (SCM s = grouping; gh_pair_p (s);
	   where += Rational (gh_scm2int (gh_car (s))) * bl,
	   s = gh_cdr (s)
	   )
	{
	  int grouplen = gh_scm2int (gh_car(s));
	  if (where == mp)
	    {
	      if (grouping_)
		{
		  programming_error ("Huh, last grouping not finished yet.");
		  continue;
		}
	      
	      grouping_ = new Spanner (get_property ("MeasureGrouping"));
	      grouping_->set_bound (LEFT, unsmob_grob (get_property ("currentMusicalColumn")));
	      announce_grob (grouping_, SCM_EOL);


	      stop_grouping_mom_ = now.main_part_ + Rational(grouplen - 1) * bl ;
	      top_engraver ()->add_moment_to_process (Moment (stop_grouping_mom_));

	      if (grouplen == 3)
		grouping_->set_grob_property ("type", ly_symbol2scm ("triangle"));
	      else
		grouping_->set_grob_property ("type", ly_symbol2scm ("bracket"));
	      
	      break ; 
	    }
	}
    }
}
Measure_grouping_engraver::Measure_grouping_engraver()
{
  grouping_ = 0;
}

ENTER_DESCRIPTION(Measure_grouping_engraver,
/* descr */       "Creates Measure_grouping objects using beatGrouping property",
/* creats*/       "MeasureGrouping",
/* accepts */     "",
/* acks  */      "note-column-interface",
/* reads */       "beatGrouping beatLength measurePosition currentMusicalColumn",
/* write */       "");
