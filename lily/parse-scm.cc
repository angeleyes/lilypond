#include <stdio.h>

#include "lily-guile.hh"
#include "parse-scm.hh"
#include "string.hh"
#include "source-file.hh"

/*
  Pass string to scm parser, evaluate one expression.
  Return result value and #chars read.

  Thanks to Gary Houston <ghouston@freewire.co.uk>

  Need guile-1.3.4 (>1.3 anyway) for ftell on str ports -- jcn
*/
SCM
internal_ly_parse_scm (Parse_start * ps, bool safe)
{
  Source_file* sf =ps->start_location_.source_file_;
  SCM port = sf->get_port ();

  int off = ps->start_location_.defined_str0_ - sf->to_str0();
  
  scm_seek (port, scm_long2num (off), scm_long2num (SEEK_SET));
  SCM from = scm_ftell (port);

  SCM form;
  SCM answer = SCM_UNSPECIFIED;

  /* Read expression from port */
  if (!SCM_EOF_OBJECT_P (form = scm_read (port)))
    {
      if (safe)
	{
	  SCM safe_module = scm_primitive_eval (ly_symbol2scm ("safe-module"));
	  answer = scm_eval (form, safe_module);
	}
      else
	answer = scm_primitive_eval (form);
    }
 
  /* Reset read_buf for scm_ftell.
     Shouldn't scm_read () do this for us?  */
  scm_fill_input (port);
  SCM to = scm_ftell (port);
  ps->nchars = gh_scm2int (to) - gh_scm2int (from);

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
  Parse_start *ps = (Parse_start*) p;
  return internal_ly_parse_scm (ps, false);
}

SCM
safe_catch_protected_parse_body (void *p)
{
  Parse_start *ps = (Parse_start*) p;
  return internal_ly_parse_scm (ps, true);
}

SCM 
parse_handler (void * data, SCM tag, SCM args)
{
  Parse_start* ps = (Parse_start*) data;

  ps->start_location_.error (_("GUILE signaled an error for the expression beginning here"));

  if (scm_ilength (args) > 2)
    scm_display_error_message (gh_cadr (args), gh_caddr (args), scm_current_error_port ());

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
protected_ly_parse_scm (Parse_start *ps, bool safe)
{
  return scm_internal_catch (ly_symbol2scm (READ_ERROR),
			     (safe ? &safe_catch_protected_parse_body
			      : catch_protected_parse_body),
			     (void*)ps,
			     &parse_handler, (void*)ps);
}

bool  parse_protect_global  = true; 

/*
  Try parsing. If failure, then return SCM_UNDEFINED.
 */
SCM
ly_parse_scm (char const* s, int *n, Input i, bool safe)
{
  
  Parse_start ps ;
  ps.str = s;
  ps.start_location_ = i;

  SCM ans = parse_protect_global ? protected_ly_parse_scm (&ps, safe)
    : internal_ly_parse_scm (&ps, safe);
  *n = ps.nchars;

  return ans;  
}

