/*
  translator-ctors.cc -- implement Translator construction

  source file of the GNU LilyPond music typesetter

  (c) 1997--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "translator.hh"
#include "scm-hash.hh"
#include "warn.hh"

/*
  should delete these after exit.
*/

Scheme_hash_table *global_translator_dict=0;

LY_DEFINE (get_all_translators,"ly:get-all-translators", 0, 0, 0,  (),
	  "Return an list of a all translator objects that may be instantiated "
	  " during a lilypond run.")
{
  SCM l = global_translator_dict ?  global_translator_dict->to_alist () : SCM_EOL;

  for (SCM s =l; gh_pair_p (s); s = gh_cdr (s))
    {
      gh_set_car_x (s, gh_cdar (s));
    }

  return l;
}

void
add_translator (Translator *t)
{
  if (!global_translator_dict)
    global_translator_dict = new Scheme_hash_table;

  SCM k= ly_symbol2scm  (classname (t));
  global_translator_dict->set (k, t->self_scm ());

  scm_gc_unprotect_object (t->self_scm ());
}

Translator*
get_translator (SCM sym)
{
  SCM v = SCM_BOOL_F;
  if (global_translator_dict)
    global_translator_dict->try_retrieve (sym, &v);

  if (v == SCM_BOOL_F)
    error (_f ("unknown translator: `%s'", ly_symbol2string (sym).to_str0 ()));

  return unsmob_translator (v);
}

