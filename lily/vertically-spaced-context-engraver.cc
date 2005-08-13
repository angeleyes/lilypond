/*
  vertically-spaced-contexts-engraver.cc -- implement Vertically_spaced_contexts_engraver

  source file of the GNU LilyPond music typesetter

  (c) 2005 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "engraver.hh"
#include "grob.hh"
#include "axis-group-interface.hh"
#include "context.hh"
#include "pointer-group-interface.hh"

class Vertically_spaced_contexts_engraver : public Engraver
{
  TRANSLATOR_DECLARATIONS (Vertically_spaced_contexts_engraver);
protected:
  virtual void initialize ();
  DECLARE_ACKNOWLEDGER (vertically_spaceable);

private:
  Grob *system_;
};

Vertically_spaced_contexts_engraver::Vertically_spaced_contexts_engraver ()
{
  system_ = 0;
}

void
Vertically_spaced_contexts_engraver::initialize ()
{
  system_ = unsmob_grob (get_property ("rootSystem"));
}

void
Vertically_spaced_contexts_engraver::acknowledge_vertically_spaceable (Grob_info gi)
{
  if (Axis_group_interface::has_interface (gi.grob ()))
    {
      SCM spaceable = get_property ("verticallySpacedContexts");
      Context *orig = gi.origin_contexts (this)[0];

      if (scm_memq (ly_symbol2scm (orig->context_name ().to_str0 ()),
		    spaceable) != SCM_BOOL_F)
	{
	  Pointer_group_interface::add_grob (system_,
					     ly_symbol2scm ("spaceable-staves"),
					     gi.grob ());
	}
    }
}

#include "translator.icc"

ADD_ACKNOWLEDGER (Vertically_spaced_contexts_engraver, vertically_spaceable);
ADD_TRANSLATOR (Vertically_spaced_contexts_engraver,
		/* doc */ "",
		/* create */ "",
		/* accept */ "",
		/* read */ "verticallySpacedContexts",
		/* write */ "verticallySpacedContexts");
