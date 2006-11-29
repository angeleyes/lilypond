/*
  grob-interface.cc -- implement graphic objects interface

  source file of the GNU LilyPond music typesetter

  (c) 2002--2006 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "grob-interface.hh"

#include "grob.hh"
#include "international.hh"
#include "protected-scm.hh"
#include "std-string.hh"
#include "string-convert.hh"
#include "warn.hh"
#include "misc.hh"

SCM add_interface (char const *cxx_name,
		    char const *descr,
		    char const *vars)
{
  string suffix ("-interface");
  string lispy_name = camel_case_to_lisp_identifier (cxx_name);
  vsize end = max (int (0), int (lispy_name.length () - suffix.length ()));
  if (lispy_name.substr (end) != suffix)
    lispy_name += suffix;

  SCM s = ly_symbol2scm (lispy_name.c_str ());
  SCM d = scm_makfrom0str (descr);
  SCM l = parse_symbol_list (vars);

  ly_add_interface (s, d, l);

  return s;
}

void
check_interfaces_for_property (Grob const *me, SCM sym)
{
  if (sym == ly_symbol2scm ("meta"))
    {
      /*
	otherwise we get in a nasty recursion loop.
      */
      return;
    }

  SCM ifs = me->interfaces ();

  SCM all_ifaces = ly_all_grob_interfaces ();
  bool found = false;
  for (; !found && scm_is_pair (ifs); ifs = scm_cdr (ifs))
    {
      SCM iface = scm_hashq_ref (all_ifaces, scm_car (ifs), SCM_BOOL_F);
      if (iface == SCM_BOOL_F)
	{
	  string msg = to_string (_f ("Unknown interface `%s'",
				      ly_symbol2string (scm_car (ifs)).c_str ()));
	  programming_error (msg);
	  continue;
	}

      found = found || (scm_c_memq (sym, scm_caddr (iface)) != SCM_BOOL_F);
    }

  if (!found)
    {
      string str = to_string (_f ("Grob `%s' has no interface for property `%s'",
				  me->name ().c_str (),
				  ly_symbol2string (sym).c_str ()));
      programming_error (str);
    }
}
