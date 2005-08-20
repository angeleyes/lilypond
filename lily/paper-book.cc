/*
  paper-book.cc -- implement Paper_book

  source file of the GNU LilyPond music typesetter

  (c) 2004--2005 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include "paper-book.hh"

#include "main.hh"
#include "output-def.hh"
#include "paper-score.hh"
#include "paper-system.hh"
#include "text-interface.hh"
#include "warn.hh"

#include "ly-smobs.icc"

Paper_book::Paper_book ()
{
  header_ = SCM_EOL;
  header_0_ = SCM_EOL;
  pages_ = SCM_BOOL_F;
  scores_ = SCM_EOL;
  performances_ = SCM_EOL;
  systems_ = SCM_BOOL_F;

  paper_ = 0;
  smobify_self ();
}

Paper_book::~Paper_book ()
{
}

IMPLEMENT_DEFAULT_EQUAL_P (Paper_book);
IMPLEMENT_SMOBS (Paper_book);
IMPLEMENT_TYPE_P (Paper_book, "ly:paper-book?");

SCM
Paper_book::mark_smob (SCM smob)
{
  Paper_book *b = (Paper_book *) SCM_CELL_WORD_1 (smob);
  if (b->paper_)
    scm_gc_mark (b->paper_->self_scm ());
  scm_gc_mark (b->header_);
  scm_gc_mark (b->header_0_);
  scm_gc_mark (b->pages_);
  scm_gc_mark (b->performances_);
  scm_gc_mark (b->scores_);
  return b->systems_;
}

int
Paper_book::print_smob (SCM smob, SCM port, scm_print_state*)
{
  Paper_book *b = (Paper_book *) SCM_CELL_WORD_1 (smob);

  scm_puts ("#<", port);
  scm_puts (classname (b), port);
  scm_puts (" ", port);
  scm_puts (">", port);
  return 1;
}

SCM
dump_fields ()
{
  SCM fields = SCM_EOL;
  for (int i = dump_header_fieldnames_global.size (); i--;)
    fields
      = scm_cons (ly_symbol2scm (dump_header_fieldnames_global[i].to_str0 ()),
		  fields);
  return fields;
}

void
Paper_book::add_score (SCM s)
{
  scores_ = scm_cons (s, scores_);
}

void
Paper_book::add_performance (SCM s)
{
  performances_ = scm_cons (s, performances_);
}

void
Paper_book::output (SCM output_channel)
{
  if (scm_is_pair (performances_))
    {
      SCM proc = ly_lily_module_constant ("paper-book-write-midis");

      scm_call_2 (proc, self_scm (), output_channel);
    }

  if (scores_ == SCM_EOL)
    return;

  /* Generate all stencils to trigger font loads.  */
  pages ();

  SCM scopes = SCM_EOL;
  if (ly_is_module (header_))
    scopes = scm_cons (header_, scopes);

  String mod_nm = "scm framework-" + output_backend_global;

  SCM mod = scm_c_resolve_module (mod_nm.to_str0 ());
  if (make_print)
    {
      SCM func = scm_c_module_lookup (mod, "output-framework");

      func = scm_variable_ref (func);
      scm_apply_0 (func, scm_list_n (output_channel,
				     self_scm (),
				     scopes,
				     dump_fields (),
				     SCM_UNDEFINED));
    }

  if (make_preview)
    {
      SCM func = scm_c_module_lookup (mod, "output-preview-framework");
      func = scm_variable_ref (func);
      scm_apply_0 (func, scm_list_n (output_channel,
				     self_scm (),
				     scopes,
				     dump_fields (),
				     SCM_UNDEFINED));
    }
}

void
Paper_book::classic_output (SCM output)
{
  /* Generate all stencils to trigger font loads.  */
  systems ();

  SCM scopes = SCM_EOL;
  if (ly_is_module (header_))
    scopes = scm_cons (header_, scopes);

  if (ly_is_module (header_0_))
    scopes = scm_cons (header_0_, scopes);

  String format = output_backend_global;
  String mod_nm = "scm framework-" + format;

  SCM mod = scm_c_resolve_module (mod_nm.to_str0 ());
  SCM func = scm_c_module_lookup (mod, "output-classic-framework");

  func = scm_variable_ref (func);
  scm_apply_0 (func, scm_list_n (output,
				 self_scm (),
				 scopes,
				 dump_fields (),
				 SCM_UNDEFINED));

  progress_indication ("\n");
}

/* TODO: resurrect more complex user-tweaks for titling?  */
Stencil
Paper_book::book_title ()
{
  SCM title_func = paper_->lookup_variable (ly_symbol2scm ("book-title"));
  Stencil title;

  SCM scopes = SCM_EOL;
  if (ly_is_module (header_))
    scopes = scm_cons (header_, scopes);

  SCM tit = SCM_EOL;
  if (ly_is_procedure (title_func))
    tit = scm_call_2 (title_func,
		      paper_->self_scm (),
		      scopes);

  if (unsmob_stencil (tit))
    title = *unsmob_stencil (tit);

  if (!title.is_empty ())
    title.align_to (Y_AXIS, UP);

  return title;
}

Stencil
Paper_book::score_title (SCM header)
{
  SCM title_func = paper_->lookup_variable (ly_symbol2scm ("score-title"));

  Stencil title;

  SCM scopes = SCM_EOL;
  if (ly_is_module (header_))
    scopes = scm_cons (header_, scopes);

  if (ly_is_module (header))
    scopes = scm_cons (header, scopes);

  SCM tit = SCM_EOL;
  if (ly_is_procedure (title_func))
    tit = scm_call_2 (title_func,
		      paper_->self_scm (),
		      scopes);

  if (unsmob_stencil (tit))
    title = *unsmob_stencil (tit);

  if (!title.is_empty ())
    title.align_to (Y_AXIS, UP);

  return title;
}

void
set_system_penalty (Paper_system *ps, SCM header)
{
  if (ly_is_module (header))
    {
      SCM force = ly_module_lookup (header, ly_symbol2scm ("breakbefore"));
      if (SCM_VARIABLEP (force)
	  && scm_is_bool (SCM_VARIABLE_REF (force)))
	{
	  ps->break_before_penalty_ = to_boolean (SCM_VARIABLE_REF (force))
	    ? -10000
	    : 10000;
	}
    }
}

void
Paper_book::add_score_title (SCM header)
{
  Stencil title = score_title (header);
  if (title.is_empty ())
    title = score_title (header_);
  if (!title.is_empty ())
    {
      Paper_system *ps = new Paper_system (title, true);
      systems_ = scm_cons (ps->self_scm (), systems_);
      ps->unprotect ();
      set_system_penalty (ps, header);
    }
}

SCM
Paper_book::systems ()
{
  if (systems_ != SCM_BOOL_F)
    return systems_;

  systems_ = SCM_EOL;
  Stencil title = book_title ();

  if (!title.is_empty ())
    {
      Paper_system *ps = new Paper_system (title, true);
      set_system_penalty (ps, header_);

      systems_ = scm_cons (ps->self_scm (), systems_);
      ps->unprotect ();
    }

  SCM page_properties
    = scm_call_1 (ly_lily_module_constant ("layout-extract-page-properties"),
		  paper_->self_scm ());

  SCM header = SCM_EOL;
  for (SCM s = scm_reverse (scores_); s != SCM_EOL; s = scm_cdr (s))
    {
      if (ly_is_module (scm_car (s)))
	{
	  header = scm_car (s);
	  if (header_0_ == SCM_EOL)
	    header_0_ = header;
	}
      else if (Music_output *mop = unsmob_music_output (scm_car (s)))

	{
	  if (Paper_score *pscore = dynamic_cast<Paper_score *> (mop))
	    {
	      add_score_title (header);

	      header = SCM_EOL;

	      SCM system_list = scm_vector_to_list (pscore->get_paper_systems ());
	      system_list = scm_reverse (system_list);
	      systems_ = scm_append (scm_list_2 (system_list, systems_));
	    }
	  else
	    {
	      /*
		Ignore MIDI
	      */
	    }
	}
      else if (scm_is_vector (scm_car (s)))
	{
	  /*
	    UGH. code dup.
	  */
	  add_score_title (header);
	  header = SCM_EOL;

	  SCM system_list = scm_vector_to_list (scm_car (s));
	  system_list = scm_reverse (system_list);
	  systems_ = scm_append (scm_list_2 (system_list, systems_));
	}
      else if (Text_interface::is_markup (scm_car (s)))
	{
	  SCM t = Text_interface::interpret_markup (paper_->self_scm (),
						    page_properties,
						    scm_car (s));
	  // FIXME: title=true?
	  Paper_system *ps = new Paper_system (*unsmob_stencil (t), true);
	  systems_ = scm_cons (ps->self_scm (), systems_);
	  ps->unprotect ();
	  // FIXME: figure out penalty.
	  //set_system_penalty (ps, scores_[i].header_);
	}
      else
	assert (0);
    }

  systems_ = scm_reverse (systems_);

  int i = 0;
  Paper_system *last = 0;
  for (SCM s = systems_; s != SCM_EOL; s = scm_cdr (s))
    {
      Paper_system *ps = unsmob_paper_system (scm_car (s));
      ps->number_ = ++i;

      if (last
	  && last->is_title ()
	  && !ps->break_before_penalty_)
	ps->break_before_penalty_ = 10000;
      last = ps;
    }

  return systems_;
}

SCM
Paper_book::pages ()
{
  if (SCM_BOOL_F != pages_)
    return pages_;

  pages_ = SCM_EOL;
  SCM proc = paper_->c_variable ("page-breaking");
  pages_ = scm_apply_0 (proc, scm_list_2 (systems (), self_scm ()));
  return pages_;
}

SCM
Paper_book::performances () const
{
  return scm_reverse (performances_);
}
