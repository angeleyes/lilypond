/*
  grid-line-span-engraver.cc --  implement Grid_line_span_engraver

  source file of the GNU LilyPond music typesetter

  (c) 2005 Han-Wen Nienhuys <hanwen@xs4all.nl>

*/

#include "engraver.hh"
#include "item.hh"
#include "grid-line-interface.hh"

class Grid_line_span_engraver : public Engraver
{
  Item *spanline_;
  Link_array<Item> lines_;

public:
  TRANSLATOR_DECLARATIONS (Grid_line_span_engraver);
protected:
  DECLARE_ACKNOWLEDGER (grid_point);
  void stop_translation_timestep ();
};

Grid_line_span_engraver::Grid_line_span_engraver ()
{
  spanline_ = 0;
}

void
Grid_line_span_engraver::acknowledge_grid_point (Grob_info i)
{
  int depth = i.origin_contexts (this).size ();
  if (depth)
    {
      Item *it = dynamic_cast<Item *> (i.grob ());
      lines_.push (it);

      if (lines_.size () >= 2 && !spanline_)
	{
	  spanline_ = make_item ("GridLine", SCM_EOL);
	  spanline_->set_parent (lines_[0], X_AXIS);
	}
    }
}

void
Grid_line_span_engraver::stop_translation_timestep ()
{
  if (spanline_)
    {
      for (int i = 0; i < lines_.size (); i++)
	Grid_line_interface::add_grid_point (spanline_, lines_[i]);

      spanline_ = 0;
    }
  lines_.set_size (0);
}

#include "translator.icc"
ADD_ACKNOWLEDGER (Grid_line_span_engraver, grid_point);
ADD_TRANSLATOR (Grid_line_span_engraver,
		/* descr */ "This engraver makes cross-staff linelines: It catches all normal "
		"line lines, and draws a single span-line across them.",
		/* creats*/ "GridLine",
		/* accepts */ "",
		/* reads */ "",
		/* write */ "");
