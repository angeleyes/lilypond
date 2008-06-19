/*
  key-engraver.cc -- implement Key_engraver

  source file of the GNU LilyPond music typesetter

  (c) 1997--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "bar-line.hh"
#include "clef.hh"
#include "context.hh"
#include "engraver.hh"
#include "item.hh"
#include "pitch.hh"
#include "protected-scm.hh"
#include "staff-symbol-referencer.hh"
#include "stream-event.hh"
#include "key-entry.hh"

#include "translator.icc"

class Key_engraver : public Engraver
{
  void create_key (bool);
  void read_event (Stream_event const *r);

  Stream_event *key_event_;
  Item *item_;
  Item *cancellation_;
public:
  TRANSLATOR_DECLARATIONS (Key_engraver);

protected:
  virtual void initialize ();
  virtual void finalize ();
  void stop_translation_timestep ();
  void process_music ();

  DECLARE_TRANSLATOR_LISTENER (key_change);
  DECLARE_ACKNOWLEDGER (clef);
  DECLARE_ACKNOWLEDGER (bar_line);
};

void
Key_engraver::finalize ()
{
}

Key_engraver::Key_engraver ()
{
  key_event_ = 0;
  item_ = 0;
  cancellation_ = 0;
}


void
Key_engraver::create_key (bool is_default)
{
  if (!item_)
    {
      item_ = make_item ("KeySignature",
			 key_event_ ? key_event_->self_scm () : SCM_EOL);

      item_->set_property ("c0-position",
			   get_property ("middleCPosition"));

      SCM last = get_property ("lastKeySignature");
      SCM key = get_property ("keySignature");
      bool extranatural = to_boolean (get_property ("extraNatural"));

      if ((to_boolean (get_property ("printKeyCancellation"))
	   || key == SCM_EOL)
	  && !scm_is_eq (last, key))
	{
	  SCM restore_scm = SCM_EOL;
	  SCM *restore_tail = &restore_scm;
	  /* run through all entries in last key signature to see which ones
	     needs to be cancelled: */
	  for (SCM s = last; scm_is_pair (s); s = scm_cdr (s))
	    {
	      Key_entry *old_entry = Key_entry::unsmob (scm_car (s));
	      Key_entry *new_entry = NULL;
	      /* run through all entries in new key signature to see if one
		 of them matches s */
	      for (SCM t = key; scm_is_pair (t); t = scm_cdr (t))
		{
		  Key_entry *entry = Key_entry::unsmob (scm_car (t));
		  if (old_entry->get_pitchclass_ref ()->get_notename () ==
		      entry->get_pitchclass_ref ()->get_notename ())
		    {
		      new_entry = entry;
		      break;
		    }
		}
	      /* Decide whether to add a key cancellation for s
	       */
	      Rational old_alter = old_entry->get_pitchclass_ref ()->get_alteration ();
	      SCM old_alterpair = old_entry->to_name_alter_pair ();
	      if (new_entry == NULL
		  || extranatural
		     && (new_entry->get_pitchclass_ref ()->get_alteration () - old_alter)*old_alter
		        < Rational (0))
		{
		  *restore_tail = scm_cons (old_alterpair, *restore_tail);
		  restore_tail = SCM_CDRLOC (*restore_tail);
		}
	    }

	  if (scm_is_pair (restore_scm))
	    {
	      cancellation_ = make_item ("KeyCancellation",
					 key_event_
					 ? key_event_->self_scm () : SCM_EOL);
	      
	      cancellation_->set_property ("alteration-alist", restore_scm);
	      cancellation_->set_property ("c0-position",
					   get_property ("middleCPosition"));
	    }
	}

      SCM key_scm = scm_list_copy (key);
      for (SCM s = key_scm; scm_is_pair (s); s = scm_cdr (s))
	{
	  Key_entry *entry = Key_entry::unsmob (scm_car (s));
	  *SCM_CARLOC (s) = entry->to_name_alter_pair ();
	}

      item_->set_property ("alteration-alist", key_scm);
    }


  if (!is_default)
    {
      SCM visibility = get_property ("explicitKeySignatureVisibility");
      item_->set_property ("break-visibility", visibility);
    }
}

IMPLEMENT_TRANSLATOR_LISTENER (Key_engraver, key_change);
void
Key_engraver::listen_key_change (Stream_event *ev)
{
  /* do this only once, just to be on the safe side.  */
  if (ASSIGN_EVENT_ONCE (key_event_, ev))
    read_event (key_event_);
}

void
Key_engraver::acknowledge_clef (Grob_info info)
{
  (void)info;
  SCM c = get_property ("createKeyOnClefChange");
  if (to_boolean (c))
    create_key (false);
}

void
Key_engraver::acknowledge_bar_line (Grob_info info)
{
  (void)info;
  if (scm_is_pair (get_property ("keySignature")))
    create_key (true);
}

void
Key_engraver::process_music ()
{
  if (key_event_
      || get_property ("lastKeySignature") != get_property ("keySignature"))
    create_key (false);
}

void
Key_engraver::stop_translation_timestep ()
{
  item_ = 0;
  context ()->set_property ("lastKeySignature", get_property ("keySignature"));
  cancellation_ = 0;
  key_event_ = 0;
}

void
Key_engraver::read_event (Stream_event const *r)
{
  SCM p = r->get_property ("pitch-alist");
  if (!scm_is_pair (p))
    return;

  SCM entries = SCM_EOL;

  SCM alist = scm_list_copy (p);
  SCM order = get_property ("keyAlterationOrder");
  for (SCM s = order;
       scm_is_pair (s) && scm_is_pair (alist); s = scm_cdr (s))
    {
      SCM head = scm_member (scm_car (s), alist);
      
      if (scm_is_pair (head))
	{
	  SCM entry_scm = Key_entry::from_name_alter_pair(scm_car (head)). smobbed_copy ();
	  entries = scm_cons (entry_scm, entries);
	  alist = scm_delete_x (scm_car (head), alist);
	}
    }

  if (scm_is_pair (alist))
    {
      bool warn = false;
      for (SCM s = alist; scm_is_pair (s); s = scm_cdr (s))
	if (ly_scm2rational (scm_cdar (s)))
	  {
	    warn = true;
	    SCM entry_scm = Key_entry::from_name_alter_pair(scm_car (s)). smobbed_copy ();
	    entries = scm_cons (entry_scm, entries);
	  }

      if (warn)
	r->origin ()->warning ("No ordering for key signature alterations");      
    }
  
  context ()->set_property ("keySignature", entries);
  context ()->set_property ("tonic",
			    r->get_property ("tonic"));
}

void
Key_engraver::initialize ()
{
  context ()->set_property ("keySignature", SCM_EOL);
  context ()->set_property ("lastKeySignature", SCM_EOL);

  Pitch p (0, 0, 0);
  context ()->set_property ("tonic", p.smobbed_copy ());
}

ADD_ACKNOWLEDGER (Key_engraver, clef);
ADD_ACKNOWLEDGER (Key_engraver, bar_line);

ADD_TRANSLATOR (Key_engraver,
		/* doc */ "",
		/* create */ "KeySignature",
		
		/* read */
		"createKeyOnClefChange "
		"explicitKeySignatureVisibility "
		"extraNatural "
		"keyAlterationOrder "
		"keySignature "
		"lastKeySignature "
		"printKeyCancellation "
		,
		
		/* write */
		"keySignature "
		"lastKeySignature "
		"tonic ")
