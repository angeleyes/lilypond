/*
  break-align-engraver.cc -- implement Break_align_engraver

  source file of the GNU LilyPond music typesetter

  (c) 1999--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/
#include "engraver.hh"
#include "protected-scm.hh"
#include "break-align-interface.hh"
#include "align-interface.hh"
#include "axis-group-interface.hh"
#include "context.hh"
#include "translator-group.hh"

#include "translator.icc"

class Break_align_engraver : public Engraver
{
  Item *align_;
  SCM column_alist_;
  Item *left_edge_;

  void add_to_group (SCM, Item *);
protected:
  void stop_translation_timestep ();
  virtual void derived_mark () const;
public:
  TRANSLATOR_DECLARATIONS (Break_align_engraver);
  DECLARE_ACKNOWLEDGER (break_aligned);
};

void
Break_align_engraver::stop_translation_timestep ()
{
  column_alist_ = SCM_EOL;

  align_ = 0;
  left_edge_ = 0;
}

Break_align_engraver::Break_align_engraver ()
{
  column_alist_ = SCM_EOL;
  left_edge_ = 0;
  align_ = 0;
}

void
Break_align_engraver::derived_mark () const
{
  scm_gc_mark (column_alist_);
}

void
Break_align_engraver::acknowledge_break_aligned (Grob_info inf)
{
  if (Item *item = dynamic_cast<Item *> (inf.grob ()))
    {
      /*
	Removed check for item->empty (X_AXIS). --hwn 20/1/04
      */
      if (item->get_parent (X_AXIS))
	return;

      SCM bp = item->get_property ("breakable");
      bool breakable = (to_boolean (bp));
      if (!breakable)
	return;

      SCM align_name = item->get_property ("break-align-symbol");
      if (!scm_is_symbol (align_name))
	return;

      if (!align_)
	{
	  align_ = make_item ("BreakAlignment", SCM_EOL);

	  Context *origin = inf.origin_contexts (this)[0];

	  Translator_group *tg = origin->implementation ();
	  Engraver *random_source = dynamic_cast<Engraver *> (unsmob_translator (scm_car (tg->get_simple_trans_list ())));

	  /*
	    Make left edge appear to come from same context as clef/bar-line etc.
	  */
	  left_edge_ = make_item_from_properties (random_source,
						  ly_symbol2scm ("LeftEdge"),
						  SCM_EOL,
						  "LeftEdge");
	  add_to_group (left_edge_->get_property ("break-align-symbol"),
			left_edge_);
	}

      add_to_group (align_name, item);
    }
}

void
Break_align_engraver::add_to_group (SCM align_name, Item *item)
{
  SCM s = scm_assoc (align_name, column_alist_);
  Item *group = 0;

  if (s != SCM_BOOL_F)
    {
      Grob *e = unsmob_grob (scm_cdr (s));
      group = dynamic_cast<Item *> (e);
    }
  else
    {
      group = make_item ("BreakAlignGroup", item->self_scm ());

      group->set_property ("break-align-symbol", align_name);
      group->set_parent (align_, Y_AXIS);

      column_alist_ = scm_assoc_set_x (column_alist_, align_name, group->self_scm ());

      Break_align_interface::add_element (align_, group);
    }
  Axis_group_interface::add_element (group, item);
}
ADD_ACKNOWLEDGER (Break_align_engraver, break_aligned);
ADD_TRANSLATOR (Break_align_engraver,
		"Align grobs with corresponding @code{break-align-symbols} into "
		"groups, and order the groups according to @code{breakAlignOrder}. "
		"The left edge of the alignment gets a separate group, with a symbol @code{left-edge}. ",
		/* create */ "BreakAlignment BreakAlignGroup LeftEdge",
		/* accept */ "",
		/* read */ "",
		/* write */ "");
