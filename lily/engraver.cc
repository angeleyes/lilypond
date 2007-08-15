/*
  engraver.cc -- implement Engraver

  Sourcefile of GNU LilyPond music type setter

  (c) 1997--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "engraver.hh"

#include "context.hh"
#include "international.hh"
#include "item.hh"
#include "lilypond-key.hh"
#include "music.hh"
#include "paper-column.hh"
#include "score-engraver.hh"
#include "spanner.hh"
#include "stream-event.hh"
#include "warn.hh"

Engraver_group *
Engraver::get_daddy_engraver () const
{
  return dynamic_cast<Engraver_group *> (get_daddy_translator ());
}

void
Engraver::announce_grob (Grob_info inf)
{
  get_daddy_engraver ()->announce_grob (inf);
}

void
Engraver::announce_end_grob (Grob_info inf)
{
  get_daddy_engraver ()->announce_grob (inf);
}

/*
  CAUSE is the object (typically a Stream_event object)  that
  was the reason for making E.
*/
void
Engraver::announce_grob (Grob *e, SCM cause)
{
  /* TODO: Remove Music code when it's no longer needed */
  if (Music *m = unsmob_music (cause))
    {
      cause = m->to_event ()->unprotect ();
    }
  if (unsmob_stream_event (cause) || unsmob_grob (cause))
    e->set_property ("cause", cause);

  Grob_info i (this, e);

  Engraver_group *g = get_daddy_engraver ();
  if (g)
    g->announce_grob (i);
}


/*
  CAUSE is the object (typically a Music object)  that
  was the reason for making E.
*/
void
Engraver::announce_end_grob (Grob *e, SCM cause)
{
  /* TODO: Remove Music code when it's no longer needed */
  if (Music *m = unsmob_music (cause))
    {
      cause = m->to_event ()->unprotect ();
    }
  if (unsmob_stream_event (cause) || unsmob_grob (cause))
    e->set_property ("cause", cause);

  Grob_info i (this, e);

  i.start_end_ = STOP;
  Engraver_group *g = get_daddy_engraver ();
  if (g)
    g->announce_grob (i);
}


Engraver::Engraver ()
{
}

#ifndef NDEBUG
static SCM creation_callback = SCM_EOL;
LY_DEFINE (ly_set_grob_creation_callback, "ly:set-grob-creation-callback",
	   1, 0, 0, (SCM cb),
	   "Specify a procedure that will be called every time a new grob "
	   "is created. The callback will receive as arguments the grob "
	   "that was created, the name of the C++ source file that caused "
	   "the grob to be created and the corresponding line number in the "
	   "C++ source file.")
{
  SCM_ASSERT_TYPE(ly_is_procedure (cb), cb, SCM_ARG1, __FUNCTION__,
		  "procedure");

  creation_callback = cb;

  return SCM_UNSPECIFIED;
}
#endif

Grob *
Engraver::internal_make_grob (SCM symbol, SCM cause, char const *name, char const *file, int line, char const *fun)
{
  (void) file;
  (void) fun;
  (void) line;
  
  SCM props = updated_grob_properties (context (), symbol);

  Object_key const *key = context ()->get_grob_key (name);
  Grob *grob = 0;

  SCM handle = scm_sloppy_assq (ly_symbol2scm ("meta"), props);
  SCM klass = scm_cdr (scm_sloppy_assq (ly_symbol2scm ("class"), scm_cdr (handle)));

  if (klass == ly_symbol2scm ("Item"))
    grob = new Item (props, key);
  else if (klass == ly_symbol2scm ("Spanner"))
    grob = new Spanner (props, key);
  else if (klass == ly_symbol2scm ("Paper_column"))
    grob = new Paper_column (props, key);

  assert (grob);
  announce_grob (grob, cause);

#ifndef NDEBUG
  if (ly_is_procedure (creation_callback))
    scm_apply_0 (creation_callback,
		 scm_list_n (grob->self_scm (), scm_makfrom0str (file),
			     scm_from_int (line), scm_makfrom0str (fun), SCM_UNDEFINED));
#endif

  return grob;
}

Item *
Engraver::internal_make_item (SCM x, SCM cause,
			      char const *name,
			      char const *file, int line, char const *fun)
{
  Item *it = dynamic_cast<Item *> (internal_make_grob (x, cause, name, file, line, fun));
  assert (it);
  return it;
}

Paper_column *
Engraver::internal_make_column (SCM x, char const *name,
				char const *file, int line, char const *fun)
{
  return dynamic_cast<Paper_column *> (internal_make_grob (x, SCM_EOL, name, file, line, fun));
}

Spanner *
Engraver::internal_make_spanner (SCM x, SCM cause, char const *name, char const *file, int line, char const *fun)
{
  Spanner *sp = dynamic_cast<Spanner *> (internal_make_grob (x, cause, name, file, line, fun));
  assert (sp);
  return sp;
}

#include "translator.icc"

ADD_TRANSLATOR (Engraver,
		"Base class for engravers. Does nothing, so it is not used.",
		"",
		"",
		"");

