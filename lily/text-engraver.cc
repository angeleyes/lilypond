/*   
  text-engraver.cc --  implement Text_engraver
  
  source file of the GNU LilyPond music typesetter
  
  (c) 1998--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  
 */
#include "directional-element-interface.hh"
#include "engraver.hh"
#include "side-position-interface.hh"
#include "item.hh"
#include "event.hh"
#include "stem.hh"
#include "rhythmic-head.hh"
#include "text-item.hh"


/**
   typeset directions that are  plain text.
 */
class Text_engraver : public Engraver
{
  Link_array<Music> reqs_;
  Link_array<Item> texts_;
public:
  TRANSLATOR_DECLARATIONS(Text_engraver);
protected:
  virtual bool try_music (Music* m);
  virtual void stop_translation_timestep ();
  virtual void process_acknowledged_grobs ();
  virtual void acknowledge_grob (Grob_info);
};

bool
Text_engraver::try_music (Music *m)
{
  if (m->is_mus_type ("text-script-event"))
    {
      reqs_.push (m);
      return true;
    }
  return false;
}

void
Text_engraver::acknowledge_grob (Grob_info inf)
{
  if (Rhythmic_head::has_interface (inf.grob_))
    {
      for (int i=0; i < texts_.size (); i++)
	{
	  Grob*t = texts_[i];
	  Side_position_interface::add_support (t,inf.grob_);

	  /*
	    ugh.
	   */
	  if (Side_position_interface::get_axis (t) == X_AXIS
	      && !t->get_parent (Y_AXIS))
	    t->set_parent (inf.grob_, Y_AXIS);
	  else if (Side_position_interface::get_axis (t) == Y_AXIS
	      && !t->get_parent (X_AXIS))
	    t->set_parent (inf.grob_, X_AXIS);
	}
    }
  
  if (Stem::has_interface (inf.grob_))
    {
      for (int i=0; i < texts_.size (); i++)
	{
	  Side_position_interface::add_support (texts_[i],inf.grob_);
	}
    }
}

void
Text_engraver::process_acknowledged_grobs ()
{
  if (texts_.size ())
    return;
  for (int i=0; i < reqs_.size (); i++)
    {
      Music * r = reqs_[i];
      
      // URG: Text vs TextScript
      Item *text = make_item ("TextScript");

      
      Axis ax = Y_AXIS;
      Side_position_interface::set_axis (text, ax);

      // Hmm
      int priority = 200;
      SCM s = text->get_grob_property ("script-priority");
      if (gh_number_p (s))
	priority = gh_scm2int (s);
      
      /* see script-engraver.cc */
      priority += i;
      
      text->set_grob_property ("script-priority", gh_int2scm (priority));

      Direction dir = to_dir (r->get_mus_property ("direction"));
      if (dir)
	set_grob_direction (text, dir);


      SCM mark = r->get_mus_property ("text");

      text->set_grob_property ("text", mark);
      announce_grob (text, r->self_scm ());
      texts_.push (text);
    }
}

void
Text_engraver::stop_translation_timestep ()
{
  for (int i=0; i < texts_.size (); i++)
    {
      Item *ti = texts_[i];
      typeset_grob (ti);
    }
  texts_.clear ();
  reqs_.clear ();
}


Text_engraver::Text_engraver ()
{
}

ENTER_DESCRIPTION(Text_engraver,
/* descr */       "Create text-scripts",
/* creats*/       "TextScript",
/* accepts */     "text-script-event",
/* acks  */      "rhythmic-head-interface stem-interface",
/* reads */       "",
/* write */       "");
