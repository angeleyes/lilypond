/*
  separating-line-group-engraver.cc -- implement Separating_line_group_engraver

  source file of the GNU LilyPond music typesetter

  (c) 1998--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "engraver.hh"

#include "separation-item.hh"
#include "paper-column.hh"
#include "output-def.hh"
#include "axis-group-interface.hh"
#include "note-spacing.hh"
#include "accidental-placement.hh"
#include "context.hh"
#include "spanner.hh"
#include "grob-array.hh"
#include "pointer-group-interface.hh"

#include "translator.icc"

struct Spacings
{
  Item *staff_spacing_;
  vector<Item*> note_spacings_;

  Spacings ()
  {
    staff_spacing_ = 0;
  }

  bool is_empty () const
  {
    return !staff_spacing_ && !note_spacings_.size ();
  }
  void clear ()
  {
    staff_spacing_ = 0;
    note_spacings_.clear ();
  }
};

class Separating_line_group_engraver : public Engraver
{
protected:
  Spacings current_spacings_;
  Spacings last_spacings_;

  DECLARE_ACKNOWLEDGER (item);
  void stop_translation_timestep ();
  void start_translation_timestep ();
public:
  TRANSLATOR_DECLARATIONS (Separating_line_group_engraver);
};

Separating_line_group_engraver::Separating_line_group_engraver ()
{
}

void
Separating_line_group_engraver::acknowledge_item (Grob_info i)
{
  Item *it = i.item ();

  if (Note_spacing::has_interface (it))
    {
      current_spacings_.note_spacings_.push_back (it);
      return;
    }

  if (Item::is_non_musical (it)
      && !current_spacings_.staff_spacing_
      && to_boolean (get_property ("createSpacing")))
    {
      Grob *col = unsmob_grob (get_property ("currentCommandColumn"));

      current_spacings_.staff_spacing_ = make_item ("StaffSpacing", SCM_EOL);
      context ()->set_property ("hasStaffSpacing", SCM_BOOL_T);

      Pointer_group_interface::add_grob (current_spacings_.staff_spacing_,
					 ly_symbol2scm ("left-items"),
					 col);
      
      if (!last_spacings_.note_spacings_.size ()
	  && last_spacings_.staff_spacing_)
	{
	  SCM ri = last_spacings_.staff_spacing_->get_object ("right-items");
	  Grob_array *ga = unsmob_grob_array (ri);
	  if (!ga)
	    {
	      SCM ga_scm = Grob_array::make_array ();
	      last_spacings_.staff_spacing_->set_object ("right-items", ga_scm);
	      ga = unsmob_grob_array (ga_scm);
	    }
	  
	  ga->clear ();
	  ga->add (col);
	}
    }
}

void
Separating_line_group_engraver::start_translation_timestep ()
{
  context ()->unset_property (ly_symbol2scm ("hasStaffSpacing"));
}

void
Separating_line_group_engraver::stop_translation_timestep ()
{
  if (!current_spacings_.is_empty ())
    last_spacings_ = current_spacings_;

  if (Item *sp = current_spacings_.staff_spacing_)
    if (Grob *col = unsmob_grob (get_property ("currentMusicalColumn")))
      Pointer_group_interface::add_grob (sp, ly_symbol2scm ("right-items"), col);

  current_spacings_.clear ();
}

ADD_ACKNOWLEDGER (Separating_line_group_engraver, item);
ADD_TRANSLATOR (Separating_line_group_engraver,
		/* doc */ "Generates objects for computing spacing parameters.",

		/* create */ "StaffSpacing",
		/* read */ "createSpacing",
		/* write */ "hasStaffSpacing"
		);
