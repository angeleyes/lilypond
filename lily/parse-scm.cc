/*
  parse-scm -- Parse a single SCM expression exactly.

  source file of the GNU LilyPond music typesetter

  (c) 2004--2005 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "parse-scm.hh"

#include <cstdio>
using namespace std;

#include "main.hh"
#include "paper-book.hh"
#include "source-file.hh"

/* Pass string to scm parser, evaluate one expression.
   Return result value and #chars read.

   Thanks to Gary Houston <ghouston@freewire.co.uk>  */
SCM
internal_ly_parse_scm (Parse_start *ps)
{
  Source_file *sf = ps->start_location_.get_source_file ();
  SCM port = sf->get_port ();

  int off = ps->start_location_.start () - sf->to_str0 ();

  scm_seek (port, scm_long2num (off), scm_long2num (SEEK_SET));
  SCM from = scm_ftell (port);

  SCM answer = SCM_UNSPECIFIED;
  SCM form = scm_read (port);

  /* Read expression from port.  */
  if (!SCM_EOF_OBJECT_P (form))
    {
      if (ps->safe_)
	{
	  static SCM module = SCM_BOOL_F;
	  if (module == SCM_BOOL_F)
	    {
	      SCM function = ly_lily_module_constant ("make-safe-lilypond-module");
	      module = scm_call_0 (function);
	    }
	  answer = scm_eval (form, module);
	}
      else
	answer = scm_primitive_eval (form);
    }

  /* Reset read_buf for scm_ftell.
     Shouldn't scm_read () do this for us?  */
  scm_fill_input (port);
  SCM to = scm_ftell (port);
  ps->nchars = scm_to_int (to) - scm_to_int (from);

  /* Don't close the port here; if we re-enter this function via a
     continuation, then the next time we enter it, we'll get an error.
     It's a string port anyway, so there's no advantage to closing it
     early. */
  // scm_close_port (port);

  return answer;
}

SCM
catch_protected_parse_body (void *p)
{
  Parse_start *ps = (Parse_start *) p;

  return internal_ly_parse_scm (ps);
}

SCM
parse_handler (void *data, SCM tag, SCM args)
{
  Parse_start *ps = (Parse_start *) data;
  (void) tag;

  ps->start_location_.error (_ ("GUILE signaled an error for the expression beginning here"));

  if (scm_ilength (args) > 2)
    scm_display_error_message (scm_cadr (args), scm_caddr (args), scm_current_error_port ());

  /*
    The following is a kludge; we should probably search for
    [a-z][0-9] (a note), and start before that.
  */
  ps->nchars = 1;

  return SCM_UNDEFINED;
}

/*
  Do some magical incantations: if not, lily will exit on the first
  GUILE error, leaving no location trace.
*/

#if GUILE_MINOR_VERSION < 7
#define READ_ERROR "misc-error"
#else
#define READ_ERROR "read-error"
#endif

SCM
protected_ly_parse_scm (Parse_start *ps)
{
  return scm_internal_catch (ly_symbol2scm (READ_ERROR),
			     &catch_protected_parse_body,
			     (void *) ps,
			     &parse_handler, (void *) ps);
}

bool parse_protect_global = true;

/* Try parsing.  Upon failure return SCM_UNDEFINED.
   FIXME: shouldn't we return SCM_UNSCPECIFIED -- jcn  */
SCM
ly_parse_scm (char const *s, int *n, Input i, bool safe)
{
  Parse_start ps;
  ps.str = s;
  ps.start_location_ = i;
  ps.safe_ = safe;

  SCM ans = parse_protect_global ? protected_ly_parse_scm (&ps)
    : internal_ly_parse_scm (&ps);
  *n = ps.nchars;

  return ans;
}

