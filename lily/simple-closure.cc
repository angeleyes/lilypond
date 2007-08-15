/*
  closure.cc -- chained closures.

  source file of the GNU LilyPond music typesetter

  (c) 2005--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>

*/
#include "simple-closure.hh"

#include "grob.hh"
#include "lily-guile.hh"

static scm_t_bits simple_closure_tag;

bool
is_simple_closure (SCM s)
{
  return (SCM_NIMP (s) && SCM_CELL_TYPE (s) == simple_closure_tag);
}

SCM
simple_closure_expression (SCM smob)
{
  assert (is_simple_closure (smob));
  return (SCM) SCM_CELL_WORD_1(smob);
}

SCM
evaluate_args (SCM delayed_argument, SCM args, bool pure, int start, int end)
{
  SCM new_args = SCM_EOL;
  SCM *tail = &new_args;
  for (SCM s = args; scm_is_pair (s); s = scm_cdr (s))
    {
      *tail = scm_cons (evaluate_with_simple_closure (delayed_argument, scm_car (s),
						      pure, start, end),
			SCM_EOL);
      if (scm_car (*tail) == SCM_UNSPECIFIED)
      	return SCM_UNSPECIFIED;
      tail = SCM_CDRLOC (*tail);
    }
  
  return new_args;
}

SCM
evaluate_with_simple_closure (SCM delayed_argument,
			      SCM expr,
			      bool pure,
			      int start,
			      int end)
{
  if (is_simple_closure (expr))
    {
      SCM inside = simple_closure_expression (expr);
      SCM args = scm_cons (delayed_argument,
			   evaluate_args (delayed_argument, scm_cdr (inside),
					  pure, start, end));
      if (scm_cdr (args) == SCM_UNSPECIFIED)
      	return SCM_UNSPECIFIED;
      if (pure)
	return call_pure_function (scm_car (inside), args, start, end);
      return scm_apply_0 (scm_car (inside), args);
    }
  else if (!scm_is_pair (expr))
    return expr;
  else if (scm_car (expr) == ly_symbol2scm ("quote"))
    return scm_cadr (expr);
  else if (ly_is_procedure (scm_car (expr)))
    {
      SCM args = evaluate_args (delayed_argument, scm_cdr (expr), pure, start, end);
      if (args == SCM_UNSPECIFIED)
      	return SCM_UNSPECIFIED;
      if (pure)
	return call_pure_function (scm_car (expr), args, start, end);
      return scm_apply_0 (scm_car (expr), args);
    }
  else
    // ugh. deviation from standard. Should print error? 
    return evaluate_args (delayed_argument, scm_cdr (expr), pure, start, end); 
  
  assert (false);
  return SCM_EOL;
}

LY_DEFINE(ly_simple_closure_p, "ly:simple-closure?",
	  1,0,0, (SCM clos),
	  "Type predicate.")
{
  return scm_from_bool (is_simple_closure (clos));
}

LY_DEFINE(ly_make_simple_closure, "ly:make-simple-closure",
	  1, 0, 0, (SCM expr),
	  "Make a simple closure. @var{expr} should be form of "
	  "@code{(@var{func} @var{a1} @var{A2} ...)}, and will be invoked "
	  "as @code{(@var{func} @var{delayed-arg} @var{a1} @var{a2} ... )}.")
{
  SCM z;

  SCM_NEWSMOB(z, simple_closure_tag, expr);
  return z;
}

LY_DEFINE(ly_eval_simple_closure, "ly:eval-simple-closure",
	  2, 2, 0, (SCM delayed, SCM closure, SCM scm_start, SCM scm_end),
	  "Evaluate a simple closure with the given delayed argument. "
	  "If start and end are defined, evaluate it purely with those "
	  "start- and end-points.")
{
  bool pure = (scm_is_number (scm_start) && scm_is_number (scm_end));
  int start = robust_scm2int (scm_start, 0);
  int end = robust_scm2int (scm_end, 0);
  SCM expr = simple_closure_expression (closure);
  return evaluate_with_simple_closure (delayed, expr, pure, start, end);
}
 
int
print_simple_closure (SCM s, SCM port, scm_print_state *)
{
  scm_puts ("#<simple-closure ", port);
  scm_display (scm_cdr (s), port);
  scm_puts (" >", port);
  return 1;
}


void init_simple_closure ()
{
  simple_closure_tag = scm_make_smob_type ("simple-closure", 0);
  scm_set_smob_mark (simple_closure_tag, scm_markcdr);
  scm_set_smob_print (simple_closure_tag, print_simple_closure);
};



ADD_SCM_INIT_FUNC (simple_closure, init_simple_closure);
