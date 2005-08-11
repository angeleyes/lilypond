/*
  hara-kiri-engraver.cc --  implement Hara_kiri_engraver

  source file of the GNU LilyPond music typesetter

  (c) 2005 Han-Wen Nienhuys <hanwen@xs4all.nl>

*/

#include "axis-group-engraver.hh"
#include "hara-kiri-group-spanner.hh"
#include "rhythmic-head.hh"
#include "spanner.hh"

#include "translator.icc"

class Hara_kiri_engraver : public Axis_group_engraver
{
protected:
  virtual Spanner *get_spanner ();
  DECLARE_ACKNOWLEDGER (grob);
  virtual void add_element (Grob *e);
  void start_translation_timestep ();
  virtual void derived_mark () const;
  SCM interesting_;
public:
  TRANSLATOR_DECLARATIONS (Hara_kiri_engraver);
};

void
Hara_kiri_engraver::derived_mark () const
{
  scm_gc_mark (interesting_);
}
  
void
Hara_kiri_engraver::start_translation_timestep ()
{
  Axis_group_engraver::start_translation_timestep ();
  interesting_ = get_property ("keepAliveInterfaces");
}


void
Hara_kiri_engraver::add_element (Grob *e)
{
  Hara_kiri_group_spanner::add_element (staffline_, e);
}

Spanner *
Hara_kiri_engraver::get_spanner ()
{
  Spanner *sp = make_spanner ("RemoveEmptyVerticalGroup", SCM_EOL);

  return sp;
}

void
Hara_kiri_engraver::acknowledge_grob (Grob_info i)
{
  Axis_group_engraver::acknowledge_grob (i);
  if (staffline_)
    {
      for (SCM s = interesting_; scm_is_pair (s); s = scm_cdr (s))
	{
	  if (i.grob ()->internal_has_interface (scm_car (s)))
	    Hara_kiri_group_spanner::add_interesting_item (staffline_, i.grob ());
	}
    }
}

Hara_kiri_engraver::Hara_kiri_engraver ()
{
  interesting_ = SCM_EOL;
}


ADD_ACKNOWLEDGER (Hara_kiri_engraver, grob);
ADD_TRANSLATOR (Hara_kiri_engraver,
		/* doc */ "Like Axis_group_engraver, but make a hara-kiri spanner, and add "
		"interesting items (ie. note heads, lyric syllables and normal rests) ",
		/* create */ "RemoveEmptyVerticalGroup",
		/* accept */ "",
		/* read */ "keepAliveInterfaces",
		/* write */ "");

