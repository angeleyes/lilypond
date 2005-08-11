/*
  rhythmic-column-engraver.cc -- implement Rhythmic_column_engraver

  source file of the GNU LilyPond music typesetter

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "engraver.hh"
#include "rhythmic-head.hh"
#include "stem.hh"
#include "note-column.hh"
#include "dot-column.hh"
#include "pointer-group-interface.hh"

#include "translator.icc"

/*
  this engraver  glues together stems, rests and note heads into a NoteColumn
  grob.

  It also generates spacing objects.  Originally, we have tried to
  have the spacing functionality at different levels.

  - by simply using the sequence of Separation-item as
  spacing-sequences (at staff level). Unfortunately, this fucks up if
  there are different kinds of tuplets in different voices (8th and
  8ths triplets combined made the program believe there were 1/12 th
  notes.).

  Doing it in a separate engraver using timing info is generally
  complicated (start/end time management), and fucks up if a voice
  changes staff.

  Now we do it from here again. This has the problem that voices can
  appear and disappear at will, leaving lots of loose ends (the note
  spacing engraver don't know where to connect the last note of the
  voice on the right with), but we don't complain about those, and let
  the default spacing do its work.
*/

class Rhythmic_column_engraver : public Engraver
{
  Link_array<Grob> rheads_;
  Grob *stem_;
  Grob *note_column_;
  Grob *dotcol_;

  Grob *last_spacing_;
  Grob *spacing_;

  void add_spacing_item (Grob*);
  
  TRANSLATOR_DECLARATIONS (Rhythmic_column_engraver);
protected:

  DECLARE_ACKNOWLEDGER (dot_column);
  DECLARE_ACKNOWLEDGER (stem);
  DECLARE_ACKNOWLEDGER (rhythmic_head);
  void process_acknowledged ();
  void stop_translation_timestep ();
};

Rhythmic_column_engraver::Rhythmic_column_engraver ()
{
  spacing_ = 0;
  last_spacing_ = 0;

  stem_ = 0;
  note_column_ = 0;
  dotcol_ = 0;
}

void
Rhythmic_column_engraver::add_spacing_item (Grob *g)
{
  if (spacing_)
    {
      Pointer_group_interface::add_grob (spacing_,
					 ly_symbol2scm ("left-items"),
					 g);

      if (last_spacing_)
	{
	  Pointer_group_interface::add_grob (last_spacing_,
					     ly_symbol2scm ("right-items"),
					     g);
	}
    }
}
void
Rhythmic_column_engraver::process_acknowledged ()
{
  if (rheads_.size ())
    {
      if (!note_column_)
	{
	  note_column_ = make_item ("NoteColumn", rheads_[0]->self_scm ());

	  spacing_ = make_item ("NoteSpacing", SCM_EOL);
	  add_spacing_item (note_column_);
	}

      for (int i = 0; i < rheads_.size (); i++)
	{
	  if (!rheads_[i]->get_parent (X_AXIS))
	    Note_column::add_head (note_column_, rheads_[i]);
	}
      rheads_.set_size (0);
    }

  if (note_column_)
    {
      if (dotcol_
	  && !dotcol_->get_parent (X_AXIS))
	{
	  Note_column::set_dotcol (note_column_, dotcol_);
	}

      if (stem_
	  && !stem_->get_parent (X_AXIS))
	{
	  Note_column::set_stem (note_column_, stem_);
	  stem_ = 0;
	}
    }
}

void
Rhythmic_column_engraver::acknowledge_stem (Grob_info i)
{
  stem_ = i.grob();
}

void
Rhythmic_column_engraver::acknowledge_rhythmic_head (Grob_info i)
{
  rheads_.push (i.grob ());
}

void
Rhythmic_column_engraver::acknowledge_dot_column (Grob_info i)
{
  dotcol_ = i.grob ();
}

void
Rhythmic_column_engraver::stop_translation_timestep ()
{
  note_column_ = 0;

  if (spacing_)
    {
      last_spacing_ = spacing_;
      spacing_ = 0;
    }

  dotcol_ = 0;
  stem_ = 0;
}


ADD_ACKNOWLEDGER (Rhythmic_column_engraver, dot_column);
ADD_ACKNOWLEDGER (Rhythmic_column_engraver, stem);
ADD_ACKNOWLEDGER (Rhythmic_column_engraver, rhythmic_head);

ADD_TRANSLATOR (Rhythmic_column_engraver,
		/* doc */ "Generates NoteColumn, an objects that groups stems, noteheads and rests.",
		/* create */ "NoteColumn NoteSpacing",
		/* accept */ "",
		/* read */ "",
		/* write */ "");
