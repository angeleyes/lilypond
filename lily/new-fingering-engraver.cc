/*   
  fingering-engraver.cc --  implement New_fingering_engraver
  
  source file of the GNU LilyPond music typesetter
  
  (c) 1998--2003 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  
 */

#include "warn.hh"
#include "engraver.hh"
#include "side-position-interface.hh"
#include "item.hh"
#include "event.hh"
#include "stem.hh"
#include "rhythmic-head.hh"
#include "self-alignment-interface.hh"
#include "script.hh"
#include "stem.hh"

struct Finger_tuple
{
  Grob *head_;
  Grob *script_;
  Music *note_event_;
  Music *finger_event_;
  SCM description_;
  int position_;

  Finger_tuple ()
  {
    position_ = 0;
    head_ = script_ = 0;
    note_event_ = finger_event_ = 0;
    description_ = SCM_EOL;
  }
  static int compare (Finger_tuple const & c1, Finger_tuple const & c2)
  {
    return c1.position_-  c2.position_;
  }
	       
};

class New_fingering_engraver : public Engraver
{
  Array<Finger_tuple> fingerings_;
  Array<Finger_tuple> articulations_;
  Link_array<Grob> heads_;
  Grob *stem_;
  
public:
  TRANSLATOR_DECLARATIONS(New_fingering_engraver);
protected:
  virtual void stop_translation_timestep ();
  virtual void acknowledge_grob (Grob_info);
  void add_fingering (Grob*, Music*,Music*);
  void add_script  (Grob*, Music*,Music*);
  void position_scripts();
};

void
New_fingering_engraver::acknowledge_grob (Grob_info inf)
{
  if (Rhythmic_head::has_interface (inf.grob_))
    {
      Music * note_ev =inf.music_cause ();

      SCM arts = note_ev->get_mus_property ("articulations");

      for (SCM s = arts; gh_pair_p (s); s = gh_cdr  (s))
	{
	  Music * m = unsmob_music (gh_car (s));

	  if (!m)
	    continue;
	  

	  if (m->is_mus_type ("fingering-event"))
	    {
	      add_fingering (inf.grob_ , m, note_ev);
	    }
	  else if (m->is_mus_type ("text-script-event"))
	    {
	      m->origin ()->warning ("Can not add text scripts to individual note heads");
	    }
	  else if (m->is_mus_type ("script-event"))
	    {
	      add_script (inf.grob_, m, note_ev);
	    }
	}

      heads_.push (inf.grob_);
    }
  else if (Stem::has_interface (inf.grob_))
    {
      stem_ = inf.grob_;
    }
}

extern Grob *make_script_from_event (SCM * descr, Translator_group*tg, Music * event,
				     int index);
void
New_fingering_engraver::add_script (Grob * head,
				    Music * event,
				    Music * head_event)
{
  Finger_tuple ft ;

  Grob * g=  make_script_from_event (&ft.description_, daddy_trans_, event, 0);
  if (g)
    {
      ft.script_ =g ;
      
      articulations_.push (ft);
      announce_grob (g, event->self_scm ());
  
 
      ft.script_->set_parent (head, X_AXIS);
    }
}				    
				    

void
New_fingering_engraver::add_fingering (Grob * head,
				       Music * event,
				       Music *hevent)
{
  Finger_tuple ft;

  ft.script_ = new Item (get_property ("Fingering"));
  announce_grob (ft.script_, event->self_scm());
  
  Side_position_interface::add_support (ft.script_, head);

  int d = gh_scm2int ( event->get_mus_property ("digit"));
  
  /*
    TODO:
    
    Should add support for thumb.  It's a little involved, since
    the thumb lives in a different font. Maybe it should be moved?
    
   */
  if (d > 5)
    {
      /*
	music for the softenon children? 
       */
      event->origin()->warning (_("music for the martians."));
    }
  SCM sstr = scm_number_to_string (gh_int2scm (d), gh_int2scm (10)) ;
  ft.script_->set_grob_property ("text", sstr);
       
  ft.finger_event_ = event;
  ft.note_event_ = hevent;
  ft.head_ = head;

  fingerings_.push (ft);
}

void
New_fingering_engraver::position_scripts ()
{

  /*
    This is not extremely elegant, but we have to do a little
    formatting here, because the parent/child relations should be
    known before we move on to the next time step.

    A more sophisticated approach would be to set both X and Y parents
    to the note head, and write a more flexible function for
    positioning the fingerings, setting both X and Y coordinates.
  */
  for (int i = 0; i < fingerings_.size(); i++)
    {      
      fingerings_[i].position_ = gh_scm2int (fingerings_[i].head_ -> get_grob_property( "staff-position"));
    }

  for (int i = fingerings_.size(); i--;)
    for (int j = heads_.size() ; j--;)
      Side_position_interface::add_support (fingerings_[i].script_, heads_[j]);
    
  Array<Finger_tuple> up, down, horiz;
  for (int i = fingerings_.size(); i--;)
    {
      SCM d = fingerings_[i].finger_event_->get_mus_property ("direction");
      if (to_dir (d))
	{
	  ((to_dir (d) == UP) ? up : down ).push (fingerings_[i]);
	  fingerings_.del (i);
	}
    }
  
  fingerings_.sort (&Finger_tuple::compare);
  SCM orientations = get_property ("fingeringOrientations");

  bool up_p = scm_memq (ly_symbol2scm ("up"), orientations) != SCM_BOOL_F;
  bool down_p = scm_memq (ly_symbol2scm ("down"), orientations) != SCM_BOOL_F;
  bool left_p = scm_memq (ly_symbol2scm ("left"), orientations) != SCM_BOOL_F;
  bool right_p = scm_memq (ly_symbol2scm ("right"), orientations) != SCM_BOOL_F;
  Direction hordir = (right_p) ? RIGHT : LEFT;
  if (left_p || right_p)
    {
      if (up_p && !up.size () && fingerings_.size ())
	up.push (fingerings_.pop());

      if (down_p && !down.size () && fingerings_.size())
	{
	  down.push (fingerings_[0]);
	  fingerings_.del(0);
	}

      horiz.concat (fingerings_);
    }
  else if (up_p && down_p)
    {
      int center = fingerings_.size() / 2;
      down.concat (fingerings_.slice (0,center));
      up.concat (fingerings_.slice (center, fingerings_.size()));
    }
  else if (up_p)
    {
      up.concat (fingerings_);
      fingerings_ .clear ();
    }
  else
    {
      if (!down_p)
	warning(_ ("Fingerings are also not down?! Putting them down anyway."));
      down.concat (fingerings_);
      fingerings_.clear();
    }
  
  for (int i = 0; i < horiz.size(); i++)
    {
      Finger_tuple ft = horiz[i];
      Grob* f = ft.script_;
      f->set_parent (ft.head_, X_AXIS);
      f->set_parent (ft.head_, Y_AXIS);
      f->add_offset_callback (Self_alignment_interface::centered_on_parent_proc, Y_AXIS);
      f->add_offset_callback (Self_alignment_interface::aligned_on_self_proc, Y_AXIS);
      f->add_offset_callback (Side_position_interface::aligned_side_proc, X_AXIS);

      f->set_grob_property ("direction", gh_int2scm (hordir));
      typeset_grob (f);
    }

  int finger_prio = 200;
  for (int i = 0; i < up.size(); i++)
    {
      Finger_tuple ft = up[i];
      Grob* f = ft.script_;
      f->set_parent (ft.head_, X_AXIS);
      f->set_grob_property ("script-priority",
			    gh_int2scm (finger_prio + i));
      f->add_offset_callback (Side_position_interface::aligned_side_proc, Y_AXIS);
      f->add_offset_callback (Self_alignment_interface::centered_on_parent_proc, X_AXIS);
      f->add_offset_callback (Self_alignment_interface::aligned_on_self_proc, X_AXIS);
      
      f->set_grob_property ("direction", gh_int2scm (UP));

      Side_position_interface::add_staff_support (f);
      typeset_grob (f);
    }
  
  for (int i = 0; i < down.size(); i++)
    {
      Finger_tuple ft = down[i];
      Grob* f = ft.script_;
      f->set_parent (ft.head_, X_AXIS);
      f->set_grob_property ("script-priority",
			    gh_int2scm (finger_prio + down.size() - i));

      f->add_offset_callback (Self_alignment_interface::centered_on_parent_proc, X_AXIS);
      f->add_offset_callback (Self_alignment_interface::aligned_on_self_proc, X_AXIS);
      f->add_offset_callback (Side_position_interface::aligned_side_proc, Y_AXIS);
      f->set_grob_property ("direction", gh_int2scm (DOWN));
      Side_position_interface::add_staff_support (f);
      typeset_grob (f);
    }
}

void
New_fingering_engraver::stop_translation_timestep ()
{
  if (fingerings_.size ())
    {
      position_scripts();
      fingerings_.clear ();
    }
  
  for (int i =  articulations_.size(); i--;)
    {
      Grob *sc = articulations_[i].script_;
   
      for (int j = heads_.size() ; j--;)
	Side_position_interface::add_support (sc, heads_[j]);

      if (stem_ && to_dir (sc->get_grob_property ("side-relative-direction")))
	sc->set_grob_property ("direction-source", stem_->self_scm ());
      
      SCM follow = scm_assoc (ly_symbol2scm ("follow-into-staff"), articulations_[i].description_);
      if (gh_pair_p (follow) && to_boolean (gh_cdr (follow)))
	sc->add_offset_callback (Side_position_interface::quantised_position_proc, Y_AXIS);
      else
	Side_position_interface::add_staff_support (sc);
      typeset_grob (sc);
    }

  stem_ = 0;
  heads_.clear ();
  articulations_.clear();
}


New_fingering_engraver::New_fingering_engraver()
{
  stem_ = 0;  
}

ENTER_DESCRIPTION(New_fingering_engraver,
/* descr */       "Create fingering-scripts for notes in a new chord.",
/* creats*/       "Fingering",
/* accepts */     "",
/* acks  */       "rhythmic-head-interface stem-interface",
/* reads */       "fingeringOrientations",
/* write */       "");
