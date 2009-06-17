/*
  staff-grouper-engraver.cc -- implement Staff_grouper_engraver

  source file of the GNU LilyPond music typesetter

  (c) 2009 Joe Neeman <joeneeman@gmail.com>
*/

#include "axis-group-interface.hh"
#include "context.hh"
#include "engraver.hh"
#include "grob-array.hh"
#include "paper-column.hh"
#include "pointer-group-interface.hh"
#include "spanner.hh"
#include "vertical-align-engraver.hh"

#include "translator.icc"

class Staff_grouper_engraver : public Vertical_align_engraver
{
public:
  TRANSLATOR_DECLARATIONS (Staff_grouper_engraver);

protected:
  virtual void make_alignment ();
  virtual void add_element (Grob*);
};

ADD_TRANSLATOR (Staff_grouper_engraver,
		/* doc */
		"Catch vertical axis groups (staves, lyrics lines, etc.) and place"
		" them in a StaffGrouper. Their order in the StaffGrouper should be"
		" the same as their order in a VerticalAlignment.",

		/* create */
		"StaffGrouper ",

		/* read */
		"alignAboveContext "
		"alignBelowContext ",

		/* write */
		""
		);

Staff_grouper_engraver::Staff_grouper_engraver ()
{
}

void
Staff_grouper_engraver::make_alignment ()
{
  valign_ = make_spanner ("StaffGrouper", SCM_EOL);
  valign_->set_bound (LEFT, unsmob_grob (get_property ("currentCommandColumn")));
  Pointer_group_interface::set_ordered (valign_, ly_symbol2scm ("elements"), true);
}

void
Staff_grouper_engraver::add_element (Grob *elt)
{
  Pointer_group_interface::add_grob (valign_, ly_symbol2scm ("elements"), elt);
  elt->set_object ("staff-grouper", valign_->self_scm ());
}


ADD_ACKNOWLEDGER (Staff_grouper_engraver, axis_group);
