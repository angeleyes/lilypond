/*
  grob-interface.cc -- implement graphic objects interface

  source file of the GNU LilyPond music typesetter

  (c) 2002--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "protected-scm.hh"
#include "grob-interface.hh"
#include "lily-guile.hh"
#include "grob.hh"
#include "warn.hh"

Protected_scm all_ifaces;

void add_interface (const char * symbol,
		    const char * descr,
		    const char * vars)
{
  SCM s = ly_symbol2scm (symbol);
  SCM d = scm_makfrom0str (descr);
  SCM l = parse_symbol_list (vars);

  ly_add_interface (s,d,l);
}


LY_DEFINE (ly_add_interface, "ly:add-interface", 3,0,0, (SCM a, SCM b, SCM c),
	  "Add an interface description.")
{
  SCM_ASSERT_TYPE (ly_c_symbol_p (a), a, SCM_ARG1, __FUNCTION__, "symbol");
  SCM_ASSERT_TYPE (ly_c_string_p (b), b, SCM_ARG2, __FUNCTION__, "string");  
  SCM_ASSERT_TYPE (ly_c_list_p (c), c, SCM_ARG3, __FUNCTION__, "list of syms");    
  if (!ly_c_vector_p (all_ifaces))
    all_ifaces = scm_make_vector (scm_int2num (40), SCM_EOL);

  SCM entry = scm_list_n (a, b, c, SCM_UNDEFINED);

  scm_hashq_set_x (all_ifaces, a, entry);

  return SCM_UNSPECIFIED;
}


LY_DEFINE (ly_all_grob_interfaces, "ly:all-grob-interfaces",
	  0,0,0, (),
	  "Get a hash table with all interface descriptions.")
{
  return all_ifaces;
}


void
check_interfaces_for_property (Grob const *me, SCM sym)
{
  if (sym == ly_symbol2scm ("meta"))
    {
      /*
	otherwise we get in a nasty recursion loop.
       */
      return ;

    }
  SCM ifs = me->get_property ("interfaces");

  bool found = false;
  for (; !found && ly_c_pair_p (ifs); ifs =ly_cdr (ifs))
    {
      SCM iface = scm_hashq_ref (all_ifaces , ly_car (ifs), SCM_BOOL_F);
      if (iface == SCM_BOOL_F)
	{
	  String msg = to_string (_f ("Unknown interface `%s'",
				      ly_symbol2string (ly_car (ifs)).to_str0 ()));
	  programming_error (msg);
	  continue;
	}

      found= found || (scm_c_memq (sym, ly_caddr (iface)) != SCM_BOOL_F);
    }

  if (!found)
    {
      String str = to_string (_f ("Grob `%s' has no interface for property `%s'",
				  me->name ().to_str0 (),
				  ly_symbol2string (sym).to_str0 ()));
      programming_error (str);
    }
}
