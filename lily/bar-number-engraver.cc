/*
  bar-number-engraver.cc -- implement Bar_number_engraver

  source file of the GNU LilyPond music typesetter

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "paper-column.hh"
#include "output-def.hh"
#include "side-position-interface.hh"
#include "engraver.hh"
#include "context.hh"
#include "grob-array.hh"

#include "translator.icc"

/*
  TODO: detect the top staff (stavesFound), and acknowledge staff-group
  system-start-delims. If we find these, and the top staff is in the
  staff-group, add padding to the bar number.
*/

class Bar_number_engraver : public Engraver
{
protected:
  Item *text_;
protected:
  void stop_translation_timestep ();
  DECLARE_ACKNOWLEDGER (break_aligned);
  void process_music ();
  void create_items ();
  TRANSLATOR_DECLARATIONS (Bar_number_engraver);
};

void
Bar_number_engraver::process_music ()
{
  // todo include (&&!time->cadenza_b_)

  SCM wb = get_property ("whichBar");

  if (scm_is_string (wb))
    {
      Moment mp (robust_scm2moment (get_property ("measurePosition"), Moment (0)));
      if (mp.main_part_ == Rational (0))
	{
	  SCM bn = get_property ("currentBarNumber");
	  SCM proc = get_property ("barNumberVisibility");
	  if (scm_is_number (bn) && ly_is_procedure (proc)
	      && to_boolean (scm_call_1 (proc, bn)))
	    {
	      create_items ();
	      // guh.
	      text_->set_property
		("text", scm_number_to_string (bn, scm_from_int (10)));
	    }
	}
    }
}

Bar_number_engraver::Bar_number_engraver ()
{
  text_ = 0;
}

void
Bar_number_engraver::acknowledge_break_aligned (Grob_info inf)
{
  Grob *s = inf.grob ();
  if (text_
      && dynamic_cast<Item *> (s)
      && s->get_property ("break-align-symbol") == ly_symbol2scm ("left-edge"))
    {
      /*
	By default this would land on the Paper_column -- so why
	doesn't it work when you leave this out?  */
      text_->set_parent (s, X_AXIS);
    }
}

void
Bar_number_engraver::stop_translation_timestep ()
{
  if (text_)
    {
      text_->set_object ("side-support-elements",
			 grob_list_to_grob_array (get_property ("stavesFound")));
      text_ = 0;
    }
}

void
Bar_number_engraver::create_items ()
{
  if (text_)
    return;

  text_ = make_item ("BarNumber", SCM_EOL);
  Side_position_interface::set_axis (text_, Y_AXIS);
}

ADD_TRANSLATOR (Bar_number_engraver,
		/* doc */ "A bar number is created whenever measurePosition "
		"is zero and there is a bar line. It is \n"
		"put on top of all staves, and appears only at  left side of the staff. "
		"The staves are taken from @code{stavesFound}, which is maintained by "
		"@code{@ref{Staff_collecting_engraver}}. ",

		/* create */ "BarNumber",
		/* accept */ "",
		/* read */
		"currentBarNumber "
		"whichBar "
		"stavesFound "
		"barNumberVisibility",
		/* write */ "");
