/*
  my-lily-parser.cc -- implement My_lily_parser

  source file of the GNU LilyPond music typesetter

  (c)  1997--2003 Han-Wen Nienhuys <hanwen@cs.uu.nl>
       Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include "my-lily-parser.hh"
#include "my-lily-lexer.hh"
#include "warn.hh"
#include "main.hh"
#include "parser.hh"
#include "input-file-results.hh"
#include "scm-hash.hh"

My_lily_parser::My_lily_parser (Input_file_results * source)
{
  input_file_ = source;
  lexer_ = 0;
  default_duration_ = Duration (2,0);
  error_level_ = 0;
  last_beam_start_ = SCM_EOL;

  default_header_ =0;
}

My_lily_parser::~My_lily_parser ()
{
  delete lexer_;
  if (default_header_)
    scm_gc_unprotect_object (default_header_->self_scm());
}


void
My_lily_parser::parse_file (String init, String in_file, String out_file)
{
  lexer_ = new My_lily_lexer;
  output_basename_ = out_file;
  
  lexer_->main_input_name_ = in_file;

  progress_indication (_ ("Parsing..."));

  set_yydebug (0);
  lexer_->new_input (init, &input_file_->sources_);
  do_yyparse ();

  progress_indication ("\n");
  
  if (!define_spots_.empty ())
    {
      define_spots_.top ().warning (_ ("Braces don't match"));
      error_level_ = 1;
    }

  input_file_->inclusion_names_ = lexer_->filename_strings_;

  error_level_ = error_level_ | lexer_->errorlevel_; // ugh naming.
}

void
My_lily_parser::push_spot ()
{
  define_spots_.push (here_input ());
}

char const *
My_lily_parser::here_str0 () const
{
  return lexer_->here_str0 ();
}

void
My_lily_parser::parser_error (String s)
{
  here_input ().error (s);
  error_level_ = 1;
  exit_status_global = 1;
}



Input
My_lily_parser::pop_spot ()
{
  return define_spots_.pop ();
}

Input
My_lily_parser::here_input () const
{
  /*
    Parsing looks ahead , so we really want the previous location of the
    lexer, not lexer_->here_input().
   */
  /*
    Actually, that gets very icky when there are white space, because
    the line-numbers are all wrong.  Let's try the character before
    the current token. That gets the right result for
    note/duration stuff, but doesn't mess up for errors in the 1st token of the line. 
    
   */
  Input hi (lexer_->here_input ());

  char const * bla = hi.defined_str0_;
  if (hi.line_number () > 1
      || hi.column_number () > 1)
    bla --;
  
  return Input (hi.source_file_, bla);
}

#include "paper-def.hh"
#include "context-def.hh"

My_lily_parser * current_parser;

MAKE_SCHEME_CALLBACK (My_lily_parser,paper_description, 0);

SCM
My_lily_parser::paper_description ()
{
  My_lily_parser * me = current_parser;

  Music_output_def *id = unsmob_music_output_def (me->lexer_->lookup_identifier ("$defaultpaper"));
  Paper_def *p = dynamic_cast<Paper_def*> (id->clone ());

  SCM al = p->translator_tab_->to_alist ();
  SCM l = SCM_EOL;
  for (SCM s = al ; gh_pair_p (s); s = ly_cdr (s))
    {
      Context_def * td = unsmob_context_def (ly_cdar (s));
      l = gh_cons (gh_cons (ly_caar (s), td->to_alist ()),  l);
    }
  return l;  
}
  

