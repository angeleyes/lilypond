/*   
  repeat-acknowledge-engraver.cc --  implement Repeat_acknowledge_engraver
  
  source file of the GNU LilyPond music typesetter
  
  (c) 2000--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  
 */

#include "engraver.hh"
#include "translator-group.hh"
#include "context.hh"
#include "repeated-music.hh"

/*
  Objective:

  -- set and reset repeatCommands, so Unfolded_repeat_iterator knows
    where to set variables.

  -- collect information passed by Unfolded_repeat_iterator for
   Bar_engraver: writes whichBar property. (TODO: check for
   interactions with timing engraver.)
  
 */
class Repeat_acknowledge_engraver : public Engraver
{
public:
  
  TRANSLATOR_DECLARATIONS (Repeat_acknowledge_engraver);
protected:
  virtual void start_translation_timestep ();
  virtual void process_music ();
  virtual void initialize ();

};

void
Repeat_acknowledge_engraver::initialize ()
{
  context ()->set_property ("repeatCommands", SCM_EOL);
}


Repeat_acknowledge_engraver::Repeat_acknowledge_engraver ()
{
}

void
Repeat_acknowledge_engraver::start_translation_timestep ()
{
  Context * tr = context ()->where_defined (ly_symbol2scm ("repeatCommands"));
  if (!tr)
    tr = context ();

  tr->set_property ("repeatCommands", SCM_EOL);
}

void
Repeat_acknowledge_engraver::process_music ()
{
  /*
    At the start of a piece, we don't print any repeat bars.
   */
  if (!now_mom ().main_part_)
    return ; 
  
  SCM cs = get_property ("repeatCommands");
  
  String s = "";
  bool start = false;
  bool end = false;
  bool volta_found = false;
  while (scm_is_pair (cs))
    {
      SCM command = scm_car (cs);
      if (command == ly_symbol2scm ("start-repeat"))
	start = true;
      else if (command == ly_symbol2scm ("end-repeat"))
	end = true;
      else if (scm_is_pair (command) && scm_car (command) == ly_symbol2scm ("volta"))
	volta_found = true;
      cs = scm_cdr (cs);      
    }

  if (start && end)
    s = ":|:";
  else if (start)
    s = "|:";
  else if (end)
    s = ":|";

  /*
    TODO: line breaks might be allowed if we set whichBar to "".
   */

  /*
    We only set the barline if we wouldn't overwrite a previously set
    barline.
   */
  SCM wb = get_property ("whichBar");
  SCM db  = get_property ("defaultBarType");
  if (!scm_is_string (wb) || ly_c_equal_p (db, wb))
    {
      if (s != "" || (volta_found && !scm_is_string (wb)))
	{
	  context ()->set_property ("whichBar", scm_makfrom0str (s.to_str0 ()));
	}
    }
}

ADD_TRANSLATOR (Repeat_acknowledge_engraver,
/* descr */       "Acknowledge repeated music, and convert the contents of "
"repeatCommands ainto an appropriate setting for whichBar.",
/* creats*/       "",
/* accepts */     "",
/* acks  */      "",
/* reads */       "repeatCommands whichBar",
/* write */       "");
