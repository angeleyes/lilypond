/*   
     piano-pedal-engraver.cc --  implement Piano_pedal_engraver
  
     source file of the GNU LilyPond music typesetter
  
     (c) 2000--2004 Jan Nieuwenhuizen <janneke@gnu.org>
  
     Chris Jackson <chris@fluffhouse.org.uk> - extended to support
     bracketed pedals.
*/

#include "engraver.hh"
#include "event.hh"
#include "grob.hh"
#include "item.hh"
#include "lily-guile.hh"
#include "side-position-interface.hh"
#include "staff-symbol-referencer.hh"
#include "item.hh"
#include "axis-group-interface.hh"
#include "context.hh"

#include "directional-element-interface.hh"
#include "note-column.hh"
#include "warn.hh"

/*
  Urgh. This engraver is too complex. rewrite. --hwn
*/

struct Pedal_info
{
  char const * name_;

  /*
    Event for currently running pedal.
  */
  Music* current_bracket_ev_;

  /*
    Event for currently starting pedal, (necessary?
    
    distinct from current_bracket_ev_, since current_bracket_ev_ only
    necessary for brackets, not for text style.
  */
  Music* start_ev_;


  
  /*
    Events that were found in this timestep.
  */
  Drul_array<Music*> event_drul_;
  Item* item_;
  Spanner* bracket_;     // A single portion of a pedal bracket
  Spanner* finished_bracket_;

  /*
    This grob contains all the pedals of the same type on the same staff
  */
  Spanner* line_spanner_;
  Spanner* finished_line_spanner_;
};


class Piano_pedal_engraver : public Engraver
{
public:
  TRANSLATOR_DECLARATIONS (Piano_pedal_engraver);
  ~Piano_pedal_engraver ();
protected:
  virtual void initialize ();
  virtual void finalize ();
  virtual bool try_music (Music*);
  virtual void stop_translation_timestep ();
  virtual void acknowledge_grob (Grob_info);
  virtual void process_music ();

private:

  Pedal_info *info_list_;

  /*
    Record a stack of the current pedal spanners, so if more than one pedal
    occurs simultaneously then extra space can be added between them.
  */
  
  Link_array<Spanner> previous_;
  void del_linespanner (Spanner*);
  
  void create_text_grobs (Pedal_info *p, bool);
  void create_bracket_grobs (Pedal_info *p, bool);
  void typeset_all (Pedal_info*p);
};


Piano_pedal_engraver::Piano_pedal_engraver ()
{
  info_list_ = 0;
}

void
Piano_pedal_engraver::initialize ()
{
  char * names [] = { "Sostenuto", "Sustain", "UnaCorda", 0  };

  info_list_ = new Pedal_info[sizeof (names)/ sizeof (const char*)]; 
  Pedal_info *p = info_list_;

  char **np = names ;
  do
    {
      p->name_ = *np;
      p->item_ = 0;
      p->bracket_ = 0;
      p->finished_bracket_ = 0;
      p->line_spanner_ = 0;
      p->finished_line_spanner_ = 0;
      p->current_bracket_ev_ = 0;
      p->event_drul_[START] = 0;
      p->event_drul_[STOP] = 0;
      p->start_ev_ = 0;

      p++;
    }
  while (* (np ++));
}

Piano_pedal_engraver::~Piano_pedal_engraver ()
{
  delete[] info_list_;
}

/*
  Urg: Code dup
  I'm a script
*/
void
Piano_pedal_engraver::acknowledge_grob (Grob_info info)
{
  for (Pedal_info*p = info_list_; p && p->name_; p ++)
    {
      if (Note_column::has_interface (info.grob_))
	{
	  if (p->line_spanner_)
	    {
	      Side_position_interface::add_support (p->line_spanner_, info.grob_);
	      add_bound_item (p->line_spanner_,info.grob_);
	    }	  
	  if (p->bracket_)
	    add_bound_item (p->bracket_,info.grob_);
	  if (p->finished_bracket_)
	    add_bound_item (p->finished_bracket_,info.grob_);		  
	}
    }
}

bool
Piano_pedal_engraver::try_music (Music *m)
{
  if (m->is_mus_type ("pedal-event"))
    {
      for (Pedal_info*p = info_list_; p->name_; p ++)
	{
	  String nm = p->name_ + String ("Event");
	  if (ly_c_equal_p (m->get_property ("name") ,
			  scm_str2symbol(nm.to_str0())))
	    {
	      Direction d = to_dir (m->get_property ("span-direction"));
	      p->event_drul_[d] = m;
	      return true;
	    }
	}
    }
  return false;
}

void
Piano_pedal_engraver::process_music ()
{
  for (Pedal_info*p = info_list_; p && p->name_; p ++)
    {
      if (p->event_drul_[STOP] || p->event_drul_[START])
	{
	  if (!p->line_spanner_)
	    {
	      String name  = String (p->name_) + "PedalLineSpanner";
	      Music * rq = (p->event_drul_[START]  ?  p->event_drul_[START]  :  p->event_drul_[STOP]);
	      p->line_spanner_ = make_spanner (name.to_str0 (), rq->self_scm ());


	      
	    }
      
	  /* Choose the appropriate grobs to add to the line spanner
	     These can be text items or text-spanners
	  */

	  /*
	    ugh, code dup, should read grob to create from other
	    property.

	    bracket: |_________/\____|
	    text:    Ped.     *Ped.  *
	    mixed:   Ped. _____/\____|
	  */


	  String prop = String ("pedal")  + p->name_ + "Style";
	  SCM style = get_property (prop.to_str0 ());

	  bool mixed = style == ly_symbol2scm ("mixed");
	  bool bracket = (mixed
			  || style == ly_symbol2scm ("bracket"));
	  bool text = (style == ly_symbol2scm ("text")
		       || mixed);
	  
	  if (text && !p->item_)
	    create_text_grobs (p, mixed);
	  if (bracket)
	    create_bracket_grobs (p, mixed);
	}
    }
}

void
Piano_pedal_engraver::create_text_grobs (Pedal_info *p, bool mixed)
{
  SCM s = SCM_EOL;
  SCM strings = get_property ( ("pedal" + String (p->name_) + "Strings").to_str0 ());

  if (scm_ilength (strings) < 3)
    {
      Music * m =       p->event_drul_[START]; 
      if (!m) m = p->event_drul_ [STOP];

      String msg = _ ("Need 3 strings for piano pedals. No pedal made. ");
      if (m)
	m->origin ()->warning (msg);
      else
	warning (msg);
      
      return ;
    }
  
  if (p->event_drul_[STOP] && p->event_drul_[START]) 
    {
      if (!mixed)
	{
	  if (!p->start_ev_)
	    {
	      p->event_drul_[STOP]->origin ()->warning (_f ("can't find start of piano pedal: `%s'",  p->name_));
	    }
	  else
	    {
	      s = ly_cadr (strings);
	    }
	  p->start_ev_ = p->event_drul_[START];
	}
    }
  else if (p->event_drul_[STOP])
    { 
      if (!mixed)
	{
	  if (!p->start_ev_)
	    {
	      p->event_drul_[STOP]->origin ()->warning (_f ("can't find start of piano pedal: `%s'", p->name_));
	    }
	  else
	    {
	      s = ly_caddr (strings);
	    }
	  p->start_ev_ = 0;
	}
    }
  else if (p->event_drul_[START])
    {
      p->start_ev_ = p->event_drul_[START];
      s = ly_car (strings);
      if (!mixed)
	{
	  /*
	    Code dup?! see below.
	  */
	  if (previous_.size ())
	    // add extra space below the previous already-occuring pedal
	    Side_position_interface::add_support (p->line_spanner_,
						  previous_.top ());
	  previous_.push (p->line_spanner_);
	}
    }
      
  if (scm_is_string (s))
    {
      String propname = String (p->name_) + "Pedal";

      p->item_ = make_item (propname.to_str0 (), (p->event_drul_[START]
						  ? p->event_drul_[START]
						  : p->event_drul_[STOP])->self_scm ());

      p->item_->set_property ("text", s);
      Axis_group_interface::add_element (p->line_spanner_, p->item_);
    }
  
  if (!mixed)
    {
      p->event_drul_[START] = 0;
      p->event_drul_[STOP] = 0;
    }
}


void
Piano_pedal_engraver::create_bracket_grobs (Pedal_info *p, bool mixed)
{
  if (!p->bracket_ && p->event_drul_[STOP])
    {
      String msg =_f ("can't find start of piano pedal bracket: `%s'", p->name_);
      p->event_drul_[STOP]->origin ()->warning (msg);
      p->event_drul_[STOP] =  0;
    }

  if (p->event_drul_[STOP])
    {
      assert (!p->finished_bracket_); 

      Grob *cmc = unsmob_grob (get_property ("currentMusicalColumn"));

      if (!p->bracket_->get_bound (RIGHT))
	p->bracket_->set_bound (RIGHT, cmc);

      /*
	Set properties so that the stencil-creating function will
	know whether the right edge should be flared ___/
      */

      if (!p->event_drul_[START])
	{
	  SCM flare = p->bracket_->get_property ("bracket-flare");
	  p->bracket_->set_property ("bracket-flare", scm_cons (ly_car (flare),
								scm_make_real (0)));
	}

      p->finished_bracket_ = p->bracket_;
      p->bracket_ = 0;
      p->current_bracket_ev_ = 0;
    }

  if (p->event_drul_[START])
    {
      p->start_ev_ = p->event_drul_[START];
      p->current_bracket_ev_ = p->event_drul_[START];

      p->bracket_  = make_spanner ("PianoPedalBracket", p->event_drul_[START]->self_scm ());

      /*
	Set properties so that the stencil-creating function will
	know whether the left edge should be flared \___
      */

      if (!p->finished_bracket_)
	{
	  SCM flare = p->bracket_->get_property ("bracket-flare");
	  p->bracket_->set_property ("bracket-flare", scm_cons (scm_make_real (0),ly_cdr (flare)));
	}


      /* Set this property for 'mixed style' pedals,    Ped._______/\ ,  
	 so the stencil function will shorten the ____ line by the length of the Ped. text.
      */

      if (mixed)
	{
	  /*
	    Mixed style: Store a pointer to the preceding text for use in
	    calculating the length of the line


	    TODO:

	    WTF is pedal-text not the bound of the object? --hwn
	  */
	  if (p->item_)
	    p->bracket_->set_property ("pedal-text", p->item_->self_scm ());
	}


      /*
	We do not use currentMusicalColumn for the left span-point.
	If the column as accidentals (eg on a different stave), the
	currentMusicalColumn is too wide, making the bracket too big.

	TODO:

	Hmm. What do we do when there are no notes when the spanner starts?

	TODO:

	what about the right span point?
	
      */
      Axis_group_interface::add_element (p->line_spanner_, p->bracket_);	      

      if (!p->event_drul_[STOP])
	{

	  /*
	    code dup. --hwn.

	    // position new pedal spanner below the current one
	    */
	  if (previous_.size ()) 
	    Side_position_interface::add_support (p->line_spanner_, previous_.top ());

	  previous_.push (p->line_spanner_);	
	}
    }

  p->event_drul_[START] = 0;
  p->event_drul_[STOP] = 0;
}

void
Piano_pedal_engraver::finalize ()
{  
  for (Pedal_info*p = info_list_; p && p->name_; p ++)
    {
      /*
	suicide?
      */
      if (p->line_spanner_
	  && !p->line_spanner_->is_live ())
	p->line_spanner_ = 0;
      
      if (p->bracket_
	  && !p->bracket_->is_live ())
	p->bracket_ = 0;
      
      if (p->bracket_)
	{
	  SCM cc = get_property ("currentCommandColumn");
	  Item *c = unsmob_item (cc);
	  if (p->line_spanner_)
	    {
	      p->line_spanner_->set_bound (RIGHT, c);
	    }
	  p->bracket_ ->set_bound (RIGHT, c);

	  p->finished_bracket_ = p->bracket_;
	  p->bracket_ = 0;
	  p->finished_line_spanner_ = p->line_spanner_;
	  p->line_spanner_ = 0;
	  typeset_all (p);
	}

      if (p->line_spanner_)
	{
	  p->finished_line_spanner_ = p->line_spanner_;
	  typeset_all (p);
	}
    }
}

void
Piano_pedal_engraver::del_linespanner (Spanner *g)
{
  int idx = previous_.find_index (g);
  if (idx >= 0)
    previous_.del (idx);
}

void
Piano_pedal_engraver::stop_translation_timestep ()
{
  for (Pedal_info*p = info_list_; p && p->name_; p ++)
    {
      if (!p->bracket_)
	{
	  p->finished_line_spanner_ = p->line_spanner_;
	  p->line_spanner_ = 0;
	  del_linespanner (p->finished_line_spanner_);
	}
      
      typeset_all (p);
    }
  

  for (Pedal_info*p = info_list_; p->name_; p ++)
    {
      p->event_drul_[STOP] = 0;
      p->event_drul_[START] = 0;
    }
}


void
Piano_pedal_engraver::typeset_all (Pedal_info * p)
{
  /*
    Handle suicide. 
  */
  if (p->finished_line_spanner_
      && !p->finished_line_spanner_->is_live ())
    p->finished_line_spanner_ = 0;
  if (p->finished_bracket_
      && !p->finished_bracket_->is_live ())
    p->finished_bracket_ = 0;


  if (p->item_)
    {
      p->item_ = 0;
    }
      
  if (p->finished_bracket_)
    {
      Grob * r = p->finished_bracket_->get_bound (RIGHT);      
      if (!r)
	{
	  p->finished_bracket_->set_bound (RIGHT, unsmob_grob (get_property ("currentMusicalColumn")));
	}

      p->finished_bracket_ = 0;
    }

  if (p->finished_line_spanner_)
    {
      Grob * l = p->finished_line_spanner_->get_bound (LEFT);
      Grob * r = p->finished_line_spanner_->get_bound (RIGHT);      
      if (!r && l)
	p->finished_line_spanner_->set_bound (RIGHT, l);
      else if (!l && r)
	p->finished_line_spanner_->set_bound (LEFT, r);
      else if (!r && !l)
	{
	  Grob * cc = unsmob_grob (get_property ("currentMusicalColumn"));
	  Item * ci = dynamic_cast<Item*> (cc);
	  p->finished_line_spanner_->set_bound (RIGHT, ci);
	  p->finished_line_spanner_->set_bound (LEFT, ci);	  
	}
      
      p->finished_line_spanner_ = 0;
    }
}

ENTER_DESCRIPTION (Piano_pedal_engraver,
		   /* descr */       "Engrave piano pedal symbols and brackets.",
		   /* creats*/       "SostenutoPedal SustainPedal UnaCordaPedal SostenutoPedalLineSpanner SustainPedalLineSpanner UnaCordaPedalLineSpanner",
		   /* accepts */     "pedal-event",
		   /* acks  */       "note-column-interface",
		   /* reads */       "currentCommandColumn "
		   "pedalSostenutoStrings pedalSustainStrings "
		   "pedalUnaCordaStrings pedalSostenutoStyle "
		   "pedalSustainStyle pedalUnaCordaStyle",
		   /* write */       "");
