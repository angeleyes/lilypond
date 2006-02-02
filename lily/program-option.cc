/*
  scm-option.cc -- implement option setting from Scheme

  source file of the GNU LilyPond music typesetter

  (c) 2001--2006  Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "program-option.hh"

#include <cstdio>
#include <cstring>
using namespace std;

#include "international.hh"
#include "main.hh"
#include "parse-scm.hh"
#include "string-convert.hh"
#include "warn.hh"

/* Write midi as formatted ascii stream? */
bool do_midi_debugging_global;
bool use_object_keys;

/*
  Backwards compatibility.
*/
bool lily_1_8_relative = false;
bool lily_1_8_compatibility_used = false;
bool profile_property_accesses = false;
/*
  crash if internally the wrong type is used for a grob property.
*/
bool do_internal_type_checking_global;

static SCM option_hash;

void internal_set_option (SCM var, SCM val)
{
  scm_hashq_set_x (option_hash, var, val);

  if (0)
    ;
  else if (var == ly_symbol2scm ("profile-property-accesses"))
    {
      profile_property_accesses = to_boolean (val);
      val = scm_from_bool (to_boolean (val));
    }
  else if (var == ly_symbol2scm ("midi-debug"))
    {
      do_midi_debugging_global = to_boolean (val);
      val = scm_from_bool (to_boolean (val));
    }
  else if (var == ly_symbol2scm ("point-and-click"))
    {
      point_and_click_global = to_boolean (val);
      val = scm_from_bool (to_boolean (val));
    }
  else if (var == ly_symbol2scm ("parse-protect"))
    {
      parse_protect_global = to_boolean (val);
      val = scm_from_bool (to_boolean (val));
    }
  else if (var == ly_symbol2scm ("internal-type-checking"))
    {
      do_internal_type_checking_global = to_boolean (val);
      val = scm_from_bool (to_boolean (val));
    }
  else if (var == ly_symbol2scm ("old-relative"))
    {
      lily_1_8_relative = to_boolean (val);
      /*  Needs to be reset for each file that uses this option.  */
      lily_1_8_compatibility_used = to_boolean (val);
      val = scm_from_bool (to_boolean (val));
    }
  else if (var == ly_symbol2scm ("object-keys"))
    {
      use_object_keys = to_boolean (val);
      val = scm_from_bool (to_boolean (val));
    }
}

ssize const HELP_INDENT = 30;
ssize const INDENT = 2;
ssize const SEPARATION = 5;

/*
  Hmmm. should do in SCM / C++  ?
*/
static std::string
get_help_string ()
{
  SCM alist = ly_hash2alist (option_hash);
  SCM convertor = ly_lily_module_constant ("scm->string");

  std::vector<std::string> opts;

  for (SCM s = alist; scm_is_pair (s); s = scm_cdr (s))
    {
      SCM sym = scm_caar (s);
      SCM val = scm_cdar (s);
      std::string opt_spec
	= String_convert::char_string (' ', INDENT)
	+ ly_symbol2string (sym)
	+ " ("
	+ ly_scm2string (scm_call_1 (convertor, val))
	+ ")";

      if (opt_spec.length () + SEPARATION > HELP_INDENT)
	{
	  opt_spec += "\n"
	    + String_convert::char_string (' ', HELP_INDENT);
	}
      else
	opt_spec += String_convert::char_string (' ', HELP_INDENT - opt_spec.length ());

      SCM opt_help_scm
	= scm_object_property (sym, ly_symbol2scm ("program-option-documentation"));
      std::string opt_help = ly_scm2string (opt_help_scm);
      replace_all (opt_help,
		   std::string ("\n"),
		   std::string ("\n")
		   + String_convert::char_string (' ', HELP_INDENT));

      opts.push_back (opt_spec + opt_help + "\n");
    }

  std::string help ("Options supported by ly:set-option\n\n");
  vector_sort (opts, string_compare);
  for (vsize i = 0; i < opts.size (); i++)
    help += opts[i];

  help += std::string ("\n");
  return help;
}

LY_DEFINE (ly_option_usage, "ly:option-usage", 0, 0, 0, (),
	   "Print ly:set-option usage")
{
  std::string help = get_help_string ();
  fputs (help.c_str (), stdout);

  exit (0);
  return SCM_UNSPECIFIED;
}

LY_DEFINE (ly_add_option, "ly:add-option", 3, 0, 0,
	   (SCM sym, SCM val, SCM description),
	   "Add a program option @var{sym} with default @var{val}.")
{
  if (!option_hash)
    {
      option_hash = scm_permanent_object (scm_c_make_hash_table (11));
    }
  SCM_ASSERT_TYPE (scm_is_symbol (sym), sym, SCM_ARG1, __FUNCTION__, "symbol");
  SCM_ASSERT_TYPE (scm_is_string (description), description,
		   SCM_ARG3, __FUNCTION__, "string");

  internal_set_option (sym, val);

  scm_set_object_property_x (sym, ly_symbol2scm ("program-option-documentation"),
			     description);

  return SCM_UNSPECIFIED;
}

LY_DEFINE (ly_set_option, "ly:set-option", 1, 1, 0, (SCM var, SCM val),
	   "Set a program option. Try setting 'help for a help string.")
{
  SCM_ASSERT_TYPE (scm_is_symbol (var), var, SCM_ARG1,
		   __FUNCTION__, "symbol");

  if (ly_symbol2scm ("help") == var)
    ly_option_usage ();

  if (val == SCM_UNDEFINED)
    val = SCM_BOOL_T;

  std::string varstr = ly_scm2string (scm_symbol_to_string (var));
  if (varstr.substr (0, 3) == std::string ("no-"))
    {
      var = ly_symbol2scm (varstr.substr (3, varstr.length () -3).c_str ());
      val = scm_from_bool (!to_boolean (val));
    }

  SCM handle = scm_hashq_get_handle (option_hash, var);
  if (handle == SCM_BOOL_F)
    warning (_f ("no such internal option: %s", varstr.c_str ()));

  internal_set_option (var, val);
  return SCM_UNSPECIFIED;
}

LY_DEFINE (ly_command_line_verbose_p, "ly:command-line-verbose?", 0, 0, 0, (),
	   "Was be_verbose_global set?")
{
  return scm_from_bool (be_verbose_global);
}

LY_DEFINE (ly_get_option, "ly:get-option", 1, 0, 0, (SCM var),
	   "Get a global option setting.")
{
  SCM_ASSERT_TYPE (scm_is_symbol (var), var,
		   SCM_ARG1, __FUNCTION__, "symbol");
  return scm_hashq_ref (option_hash, var, SCM_BOOL_F);
}
