/*
  score-engraver.cc -- implement Score_engraver

  source file of the GNU LilyPond music typesetter

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "all-font-metrics.hh"
#include "afm.hh"
#include "warn.hh"
#include "main.hh"
#include "system.hh"
#include "score-engraver.hh"
#include "paper-score.hh"
#include "paper-column.hh"
#include "output-def.hh"
#include "axis-group-interface.hh"
#include "context-def.hh"
#include "global-context.hh"
#include "open-type-font.hh"
#include "paper-column-engraver.hh"

/*
  TODO: the column creation logic is rather hairy. Revise it.
*/
Score_engraver::Score_engraver ()
{
  system_ = 0;
  pscore_ = 0;
}

void
Score_engraver::derived_mark () const
{
  if (pscore_)
    scm_gc_mark (pscore_->self_scm ());
  Score_translator::derived_mark ();
  Engraver_group_engraver::derived_mark ();
}


void
Score_engraver::prepare (Moment m)
{
  (void) m;
  
  precomputed_recurse_over_translators (context (), START_TRANSLATION_TIMESTEP, DOWN);
}

void
Score_engraver::finish ()
{
  recurse_over_translators (context (), &Translator::finalize,
			    &Translator_group::finalize,
			    UP);
}

#define MUSIC_FONT "emmentaler-20"

/*
  use start/finish?
*/
void
Score_engraver::initialize ()
{
  Font_metric *fm = all_fonts_global->find_otf (MUSIC_FONT);
  if (!fm)
    {
      error (_f ("cannot find `%s'", MUSIC_FONT ".otf")
	     + "\n"
	     + _ ("Music font has not been installed properly.")
	     + "\n"
	     + _f ("Search path `%s'", global_path.to_string ().to_str0 ())
	     + "\n"
	     + _ ("Aborting"));
    }

  pscore_ = new Paper_score (dynamic_cast<Output_def *> (context ()->get_output_def ()));
  scm_gc_unprotect_object (pscore_->self_scm ()); 

  SCM props = updated_grob_properties (context (), ly_symbol2scm ("System"));

  Object_key const *sys_key = context ()->get_grob_key ("System");
  pscore_->typeset_system (new System (props, sys_key));
  system_ = pscore_->root_system ();
  context ()->set_property ("rootSystem",  system_->self_scm ());
  
  Engraver_group_engraver::initialize ();
}

void
Score_engraver::finalize ()
{
  Score_translator::finalize ();

  typeset_all ();
}

void
Score_engraver::one_time_step ()
{
  if (!to_boolean (context ()->get_property ("skipTypesetting")))
    {
      precomputed_recurse_over_translators (context (), PROCESS_MUSIC, UP);
      Engraver_group_engraver::do_announces ();
    }

  precomputed_recurse_over_translators (context (), STOP_TRANSLATION_TIMESTEP, UP);
  typeset_all ();
}

void
Score_engraver::announce_grob (Grob_info info)
{
  announce_infos_.push (info);
  pscore_->root_system ()->typeset_grob (info.grob ());
  elems_.push (info.grob ());
}

void
Score_engraver::typeset_all ()
{
  for (int i = 0; i < elems_.size (); i++)
    {
      Grob *elem = elems_[i];

      if (!elem->get_parent (Y_AXIS))
	Axis_group_interface::add_element (system_, elem);
    }
  elems_.clear ();
}

SCM
Score_engraver::get_output ()
{
  Music_output *o = pscore_;
  return o->self_scm ();
}


/*
  UGH UGH
 */
void
Score_engraver::forbid_breaks ()
{
  for (SCM s = simple_trans_list_; scm_is_pair (s); s = scm_cdr (s))
    {
      Translator *tr = unsmob_translator (scm_car (s));
      if (Paper_column_engraver* pce = dynamic_cast<Paper_column_engraver*> (tr))
	{
	  pce->forbid_breaks ();
	}
    }
}

bool
Score_engraver::try_music (Music *m)
{
  if (Engraver_group_engraver::try_music (m))
    return true;

  return false;
}

ADD_TRANSLATOR_GROUP (Score_engraver,
		/* descr */ "Top level engraver. Takes care of generating columns and the complete  system (ie. System) "
		"\n\n "
		"This engraver decides whether a column is breakable. The default is "
		"that a column is always breakable. However, when every Bar_engraver "
		"that does not have a barline at a certain point will call "
		"Score_engraver::forbid_breaks to stop linebreaks.  In practice, this "
		"means that you can make a breakpoint by creating a barline (assuming "
		"that there are no beams or notes that prevent a breakpoint.) ",
		/* creats*/ "System PaperColumn NonMusicalPaperColumn",
		/* accepts */ "break-event",
		/* reads */ "currentMusicalColumn currentCommandColumn verticallySpacedContexts",
		/* write */ "");
