/*
  part-combine-engraver.cc -- implement PC-engraver

  source file of the GNU LilyPond music typesetter
  
  (c) 2000--2003 Jan Nieuwenhuizen <janneke@gnu.org>

  Han-Wen Nienhuys <hanwen@xs4all.nl>
  
*/

#include "engraver.hh"
#include "item.hh"
#include "text-item.hh"
#include "note-head.hh"
#include "stem.hh"
#include "side-position-interface.hh"
#include "multi-measure-rest.hh"

class Part_combine_engraver : public Engraver
{
  TRANSLATOR_DECLARATIONS(Part_combine_engraver);

protected:
  virtual void acknowledge_grob (Grob_info);
  virtual void process_music ();
  virtual void stop_translation_timestep ();
  virtual bool try_music (Music *);
private:
  Item *text_;
  Music *event_;
};

bool
Part_combine_engraver::try_music (Music*m)
{
  event_ = m;
  return true;
}

Part_combine_engraver::Part_combine_engraver ()
{
  text_ = 0;
  event_  =0;
}

void
Part_combine_engraver::process_music ()
{
  if (event_
      && to_boolean (get_property ("soloADue")))
    {
      SCM what = event_->get_mus_property ("part-combine-status");
      SCM text = SCM_EOL;
      if (what == ly_symbol2scm ("solo1"))
	text = get_property ("soloText");
      else if (what== ly_symbol2scm ("solo2"))
	text = get_property ("soloIIText");
      else if (what == ly_symbol2scm ("unisono"))
	text = get_property ("aDueText");

      if (Text_item::markup_p (text))
	{
	  text_ =  make_item ("CombineTextScript");
	  text_->set_grob_property ("text", text);
	  announce_grob (text_, event_->self_scm ());
	}
    }
}

void
Part_combine_engraver::acknowledge_grob (Grob_info i)
{
  if (text_)
    {
      if (Note_head::has_interface (i.grob_))
	{
	  Grob*t = text_;
	  Side_position_interface::add_support (t, i.grob_);
	  if (Side_position_interface::get_axis (t) == X_AXIS
	      && !t->get_parent (Y_AXIS))
	    t->set_parent (i.grob_, Y_AXIS);
	}
      if (Stem::has_interface (i.grob_))
	{
	  Side_position_interface::add_support (text_, i.grob_);
	}
    }
}

void 
Part_combine_engraver::stop_translation_timestep ()
{
  if (text_)
    {
      typeset_grob (text_);
      text_ = 0;
    }
  event_ = 0;
}

ENTER_DESCRIPTION(Part_combine_engraver,
/* descr */       "Part combine engraver for orchestral scores:		"
		  "Print markings a2, Solo, Solo II, and unisono ",
/* creats*/       "CombineTextScript",
/* accepts */     "part-combine-event",
/* acks  */       "multi-measure-rest-interface "
"slur-interface stem-interface note-head-interface"
,/* reads */       "soloADue",
/* write */       "");
