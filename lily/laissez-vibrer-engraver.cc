/*
  laissez-vibrer-engraver.cc -- implement Laissez_vibrer_engraver

  source file of the GNU LilyPond music typesetter

  (c) 2005--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>

*/


#include "engraver.hh"
#include "item.hh"
#include "pointer-group-interface.hh"
#include "stream-event.hh"

#include "translator.icc"

class Laissez_vibrer_engraver : public Engraver
{
  Stream_event *event_;
  Grob *lv_column_;
  vector<Grob*> lv_ties_;
  
  void stop_translation_timestep (); 
  DECLARE_ACKNOWLEDGER (note_head);
protected:
  DECLARE_TRANSLATOR_LISTENER (laissez_vibrer);
public:
  TRANSLATOR_DECLARATIONS (Laissez_vibrer_engraver);
};

Laissez_vibrer_engraver::Laissez_vibrer_engraver ()
{
  event_ = 0;
  lv_column_ = 0;
}

void
Laissez_vibrer_engraver::stop_translation_timestep ()
{
  event_ = 0;
  lv_column_ = 0;
  lv_ties_.clear ();
}

IMPLEMENT_TRANSLATOR_LISTENER (Laissez_vibrer_engraver, laissez_vibrer);
void
Laissez_vibrer_engraver::listen_laissez_vibrer (Stream_event *ev)
{
  ASSIGN_EVENT_ONCE (event_, ev);
}

void
Laissez_vibrer_engraver::acknowledge_note_head (Grob_info inf)
{
  if (!event_)
    return;

  if (!lv_column_)
    {
      lv_column_ = make_item ("LaissezVibrerTieColumn", event_->self_scm ());
    }
  
  Grob *lv_tie = make_item ("LaissezVibrerTie", event_->self_scm ());
  lv_tie->set_object ("note-head", inf.grob ()->self_scm ());
  
  Pointer_group_interface::add_grob (lv_column_, ly_symbol2scm ("ties"),
				     lv_tie);
  lv_tie->set_parent (lv_column_, Y_AXIS);

  lv_ties_.push_back (lv_tie);
}

ADD_ACKNOWLEDGER (Laissez_vibrer_engraver, note_head);
ADD_TRANSLATOR (Laissez_vibrer_engraver, 
		/* doc */ "Create Laissez vibrer items.",
		
		/* create */
		"LaissezVibrerTie "
		"LaissezVibrerTieColumn",
		/* read */ "",
		/* write */ "");
