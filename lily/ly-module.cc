/*
  ly-module.cc -- implement guile module stuff.

  source file of the GNU LilyPond music typesetter

  (c) 2002--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "lily-guile.hh"
#include "warn.hh"
#include "main.hh"
#include "string.hh"

#define FUNC_NAME __FUNCTION__

static int module_count;

void
ly_init_anonymous_module (void *data)
{
  (void) data;
}

SCM
ly_make_anonymous_module (bool safe)
{
  SCM mod = SCM_EOL;
  if (!safe)
    {
      String s = "*anonymous-ly-" + to_string (module_count++) + "*";
      mod = scm_c_define_module (s.to_str0 (), ly_init_anonymous_module, 0);
      ly_use_module (mod, global_lily_module);
    }
  else
    {
      SCM proc = ly_lily_module_constant ("make-safe-lilypond-module");
      mod = scm_call_0 (proc);
    }
  return mod;
}

SCM
ly_use_module (SCM mod, SCM used)
{
  SCM expr
    = scm_list_3 (ly_symbol2scm ("module-use!"),
		  mod,
		  scm_list_2 (ly_symbol2scm ("module-public-interface"),
			      used));

  return scm_eval (expr, global_lily_module);
}

#define FUNC_NAME __FUNCTION__

static SCM
module_define_closure_func (void *closure, SCM key, SCM val, SCM result)
{
  (void) result;
  SCM module = (SCM) closure;
  if (scm_variable_bound_p (val) == SCM_BOOL_T)
    scm_module_define (module, key, scm_variable_ref (val));
  return SCM_EOL;
}

/* Ugh signature of scm_internal_hash_fold () is inaccurate.  */
typedef SCM (*Hash_cl_func) ();

/*
  If a variable in changed in SRC, we DEST doesn't see the
  definitions.
*/
LY_DEFINE (ly_module_copy, "ly:module-copy",
	   2, 0, 0, (SCM dest, SCM src),
	   "Copy all bindings from module SRC into DEST.")
{
  SCM_VALIDATE_MODULE (1, src);
  scm_internal_hash_fold ((Hash_cl_func) & module_define_closure_func,
			  (void *) dest,
			  SCM_EOL, SCM_MODULE_OBARRAY (src));
  return SCM_UNSPECIFIED;
}

static SCM
accumulate_symbol (void *closure, SCM key, SCM val, SCM result)
{
  (void) closure;
  (void) val;
  return scm_cons (key, result);
}

SCM
ly_module_symbols (SCM mod)
{
  SCM_VALIDATE_MODULE (1, mod);

  SCM obarr = SCM_MODULE_OBARRAY (mod);
  return scm_internal_hash_fold ((Hash_cl_func) & accumulate_symbol,
				 NULL, SCM_EOL, obarr);
}

static SCM
entry_to_alist (void *closure, SCM key, SCM val, SCM result)
{
  (void) closure;
  if (scm_variable_bound_p (val) == SCM_BOOL_T)
    {
      return scm_cons (scm_cons (key, scm_variable_ref (val)), result);
    }
  else
    {
      programming_error ("Unbound variable in module.");
      return result;
    }
}

LY_DEFINE (ly_module2alist, "ly:module->alist",
	   1, 0, 0, (SCM mod),
	   "Dump the contents of  module @var{mod} as an alist.")
{
  SCM_VALIDATE_MODULE (1, mod);
  SCM obarr = SCM_MODULE_OBARRAY (mod);

  return scm_internal_hash_fold ((Hash_cl_func) & entry_to_alist, NULL, SCM_EOL, obarr);
}

/* Lookup SYM, but don't give error when it is not defined.  */
SCM
ly_module_lookup (SCM module, SCM sym)
{
#define FUNC_NAME __FUNCTION__
  SCM_VALIDATE_MODULE (1, module);

  return scm_sym2var (sym, scm_module_lookup_closure (module), SCM_BOOL_F);
#undef FUNC_NAME
}

/* Lookup SYM in a list of modules, which do not have to be related.
   Return the first instance. */
LY_DEFINE (ly_modules_lookup, "ly:modules-lookup",
	   2, 1, 0,
	   (SCM modules, SCM sym, SCM def),
	   "Lookup @var{sym} in the list @var{modules}, "
	   "returning the first occurence.  "
	   "If not found, return @var{default}, or @code{#f}.")
{
  for (SCM s = modules; scm_is_pair (s); s = scm_cdr (s))
    {
      SCM mod = scm_car (s);
      SCM v = ly_module_lookup (mod, sym);
      if (SCM_VARIABLEP (v) && SCM_VARIABLE_REF (v) != SCM_UNDEFINED)
	return scm_variable_ref (v);
    }

  if (def != SCM_UNDEFINED)
    return def;
  return SCM_BOOL_F;
}

void
ly_export (SCM module, SCM namelist)
{
  static SCM export_function;
  if (!export_function)
    export_function = scm_permanent_object (scm_c_lookup ("module-export!"));

  scm_call_2 (SCM_VARIABLE_REF (export_function), module, namelist);
}

void
ly_reexport_module (SCM mod)
{
  ly_export (mod, ly_module_symbols (mod));
}
