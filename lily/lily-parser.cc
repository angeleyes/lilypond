/*
  lily-parser.cc -- implement Lily_parser

  source file of the GNU LilyPond music typesetter

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include "lily-parser.hh"
#include "text-metrics.hh"
#include "book.hh"
#include "lilypond-key.hh"
#include "lily-version.hh"
#include "main.hh"
#include "lily-lexer.hh"
#include "output-def.hh"
#include "paper-book.hh"
#include "parser.hh"
#include "score.hh"
#include "file-name.hh"
#include "file-path.hh"
#include "source.hh"
#include "warn.hh"

Lily_parser::Lily_parser (Sources *sources)
{
  book_count_ = 0;
  score_count_ = 0;
  lexer_ = 0;
  sources_ = sources;
  default_duration_ = Duration (2, 0);
  error_level_ = 0;
  last_beam_start_ = SCM_EOL;

  smobify_self ();
}

Lily_parser::Lily_parser (Lily_parser const &src)
{
  book_count_ = src.book_count_;
  score_count_ = src.score_count_;
  lexer_ = 0;
  sources_ = src.sources_;
  default_duration_ = src.default_duration_;
  error_level_ = src.error_level_;
  last_beam_start_ = src.last_beam_start_;

  smobify_self ();
  if (src.lexer_)
    lexer_ = new Lily_lexer (*src.lexer_);

  scm_gc_unprotect_object (lexer_->self_scm ());
}

Lily_parser::~Lily_parser ()
{
}

#include "ly-smobs.icc"

IMPLEMENT_SMOBS (Lily_parser);
IMPLEMENT_TYPE_P (Lily_parser, "ly:lily-parser?");
IMPLEMENT_DEFAULT_EQUAL_P (Lily_parser);

SCM
Lily_parser::mark_smob (SCM s)
{
  Lily_parser *parser = (Lily_parser *) SCM_CELL_WORD_1 (s);
  return (parser->lexer_) ? parser->lexer_->self_scm () : SCM_EOL;
}

int
Lily_parser::print_smob (SCM s, SCM port, scm_print_state*)
{
  scm_puts ("#<my_lily_parser ", port);
  Lily_parser *parser = (Lily_parser *) SCM_CELL_WORD_1 (s);
  (void) parser;
  scm_puts (" >", port);
  return 1;
}

/* Process one .ly file, or book.  */
void
Lily_parser::parse_file (String init, String name, String out_name)
{
  if (output_backend_global == "tex")
    {
      try_load_text_metrics (out_name);
    }

  lexer_ = new Lily_lexer (sources_);
  scm_gc_unprotect_object (lexer_->self_scm ());
  // TODO: use $parser 
  lexer_->set_identifier (ly_symbol2scm ("parser"),
			  self_scm ());
  output_basename_ = out_name;

  lexer_->main_input_name_ = name;

  progress_indication (_ ("Parsing..."));
  progress_indication ("\n");

  set_yydebug (0);

  lexer_->new_input (init, sources_);

  File_name f (name);
  String s = global_path.find (f.base_ + ".twy");
  s = gulp_file_to_string (s, false);
  scm_eval_string (scm_makfrom0str (s.to_str0 ()));

  /* Read .ly IN_FILE, lex, parse, write \score blocks from IN_FILE to
     OUT_FILE (unless IN_FILE redefines output file name).  */
  do_yyparse ();

  if (!define_spots_.is_empty ())
    {
      define_spots_.top ().warning (_ ("Braces don't match"));
      error_level_ = 1;
    }

  error_level_ = error_level_ | lexer_->error_level_;
  lexer_ = 0;
}

void
Lily_parser::parse_string (String ly_code)
{
  Lily_lexer *parent = lexer_;
  lexer_ = (parent == 0 ? new Lily_lexer (sources_)
	    : new Lily_lexer (*parent));
  scm_gc_unprotect_object (lexer_->self_scm ());

  SCM oldmod = scm_current_module ();
  scm_set_current_module (scm_car (lexer_->scopes_));

  // TODO: use $parser 
  lexer_->set_identifier (ly_symbol2scm ("parser"),
			  self_scm ());

  lexer_->main_input_name_ = "<string>";
  lexer_->is_main_input_ = true;

  set_yydebug (0);
  lexer_->new_input (lexer_->main_input_name_, ly_code, sources_);
  do_yyparse ();

  if (!define_spots_.is_empty ())
    {
      if (define_spots_.is_empty ()
	  && !error_level_)
	programming_error ("define_spots_ don't match, but error_level_ not set.");
    }

  error_level_ = error_level_ | lexer_->error_level_;

  scm_set_current_module (oldmod);
  lexer_ = 0;
}

char const *
Lily_parser::here_str0 () const
{
  return lexer_->here_str0 ();
}

void
Lily_parser::parser_error (String s)
{
  lexer_->here_input ().error (s);
  error_level_ = 1;
}

void
Lily_parser::parser_error (Input const &i, String s)
{
  i.error (s);
  error_level_ = 1;
}

/****************************************************************/

Output_def *
get_layout (Lily_parser *parser)
{
  SCM id = parser->lexer_->lookup_identifier ("$defaultlayout");
  Output_def *layout = unsmob_output_def (id);
  layout = layout ? layout->clone () : new Output_def;
  layout->set_variable (ly_symbol2scm ("is-layout"), SCM_BOOL_T);

  return layout;
}

Output_def *
get_midi (Lily_parser *parser)
{
  SCM id = parser->lexer_->lookup_identifier ("$defaultmidi");
  Output_def *layout = unsmob_output_def (id);
  layout = layout ? layout->clone () : new Output_def;
  layout->set_variable (ly_symbol2scm ("is-midi"), SCM_BOOL_T);
  return layout;
}

Output_def *
get_paper (Lily_parser *parser)
{
  SCM id = parser->lexer_->lookup_identifier ("$defaultpaper");
  Output_def *layout = unsmob_output_def (id);

  layout = layout ? dynamic_cast<Output_def *> (layout->clone ()) : new Output_def;
  layout->set_variable (ly_symbol2scm ("is-paper"), SCM_BOOL_T);
  return layout;
}

