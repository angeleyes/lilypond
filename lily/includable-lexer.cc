/*
  includable-lexer.cc -- implement Includable_lexer

  source file of the LilyPond music typesetter

  (c)  1997--2002 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include <sstream>


#include "file-path.hh"
#include "includable-lexer.hh"
#include "source-file.hh"
#include "source.hh"
#include "warn.hh"
#include "main.hh"

#ifndef YY_BUF_SIZE
#define YY_BUF_SIZE 16384
#endif

#ifndef YY_START
#define YY_START\
 ((yy_start - 1) / 2)
#define YYSTATE YY_START
#endif

Includable_lexer::Includable_lexer ()
{
  yy_current_buffer = 0;
  allow_includes_b_ = true;
}

/** set the  new input to s, remember old file.
*/
void
Includable_lexer::new_input (String s, Sources  * global_sources)
{
  if (!allow_includes_b_)
    {
      LexerError ("include files are disallowed.");
      return;
    }
  
  Source_file * sl = global_sources->get_file (s);
  if (!sl)
    {
      String msg = _f ("can't find file: `%s'", s);
      msg += "\n";
      msg += _f ("(search path: `%s')", global_sources->path_C_->string ().to_str0 ());
      msg += "\n";
      LexerError (msg.to_str0 ());

      return;
    }
  filename_strings_.push (sl->name_string ());

  char_count_stack_.push (0);
  if (yy_current_buffer)
    state_stack_.push (yy_current_buffer);

  if (verbose_global_b)
    progress_indication (String ("[") + s);
	
  include_stack_.push (sl);

  /*
    ugh. We'd want to create a buffer from the bytes directly.

    Whoops. The size argument to yy_create_buffer is not the
    filelength but a BUFFERSIZE. Maybe this is why reading stdin fucks up.

  */
  yy_switch_to_buffer (yy_create_buffer (sl->get_istream (), YY_BUF_SIZE));

}

/*
  Unused.
 */
void
Includable_lexer::new_input (String name, String data, Sources* sources)
{
  Source_file* file = new Source_file (name, data);
  sources->add (file);
  filename_strings_.push (name);

  char_count_stack_.push (0);
  if (yy_current_buffer)
    state_stack_.push (yy_current_buffer);

  if (verbose_global_b)
    progress_indication (String ("[") + name);
  include_stack_.push (file);

  yy_switch_to_buffer (yy_create_buffer (file->get_istream (), YY_BUF_SIZE));
}

/** pop the inputstack.  conceptually this is a destructor, but it
  does not destruct the Source_file that Includable_lexer::new_input creates.  */
bool
Includable_lexer::close_input ()
{
  include_stack_.pop ();
  char_count_stack_.pop ();
  if (verbose_global_b)
    progress_indication ("]");
  yy_delete_buffer (yy_current_buffer);
  yy_current_buffer = 0;
  if (state_stack_.empty ())
    {
      yy_current_buffer = 0;
      return false;
    }
  else
    {
      yy_switch_to_buffer (state_stack_.pop ());
      return true;
    }
}

char const*
Includable_lexer::here_str0 () const
{
  if (include_stack_.empty ())
    return 0;
  return include_stack_.top ()->to_str0 () + char_count_stack_.top ();
}

Includable_lexer::~Includable_lexer ()
{
  while (!include_stack_.empty ())
    {
      close_input ();
    }
}
/**
  Since we don't create the buffer state from the bytes directly, we
  don't know about the location of the lexer. Add this as a
  YY_USER_ACTION */
void
Includable_lexer::add_lexed_char (int count)
{
  char_count_stack_.top () += count;
}

Source_file*
Includable_lexer::get_source_file () const
{
  if (include_stack_.empty ())
    return 0;
  else
    return include_stack_.top ();
}
