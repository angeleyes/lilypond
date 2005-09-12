/*
  scm-option.cc -- implement option setting from Scheme

  source file of the GNU LilyPond music typesetter

  (c) 2001--2005  Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "program-option.hh"

#include <cstdio>
#include <cstring>
using namespace std;

#include "string-convert.hh"
#include "protected-scm.hh"
#include "parse-scm.hh"
#include "warn.hh"
#include "main.hh"

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

Protected_scm option_hash_;

void internal_set_option (SCM var, SCM val)
{
  scm_hashq_set_x (option_hash_, var, val);

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

const int HELP_INDENT = 30;
const int INDENT = 2;
const int SEPARATION = 5;

/*
  Hmmm. should do in SCM / C++  ?
*/
static String
get_help_string ()
{
  SCM alist = ly_hash2alist (option_hash_);
  SCM convertor = ly_lily_module_constant ("scm->string");

  Array<String> opts;

  for (SCM s = alist; scm_is_pair (s); s = scm_cdr (s))
    {
      SCM sym = scm_caar (s);
      SCM val = scm_cdar (s);
      String opt_spec
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
      String opt_help = ly_scm2string (opt_help_scm);
      opt_help.substitute (String ("\n"),
			   String ("\n")
			   + String_convert::char_string (' ', HELP_INDENT));

      opts.push (opt_spec + opt_help + "\n");
    }

  String help ("Options supported by ly:set-option\n\n");
  opts.sort (String::compare);
  for (int i = 0; i < opts.size (); i++)
    help += opts[i];

  help += String ("\n");
  return help;
}

LY_DEFINE (ly_option_usage, "ly:option-usage", 0, 0, 0, (),
	   "Print ly:set-option usage")
{
  String help = get_help_string ();
  fputs (help.to_str0 (), stdout);

  exit (0);
  return SCM_UNSPECIFIED;
}

LY_DEFINE (ly_add_option, "ly:add-option", 3, 0, 0,
	   (SCM sym, SCM val, SCM description),
	   "Add a program option @var{sym} with default @var{val}.")
{
  if (scm_hash_table_p (option_hash_) == SCM_BOOL_F)
    option_hash_ = scm_c_make_hash_table (11);

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

  String varstr = ly_scm2string (scm_symbol_to_string (var));
  if (varstr.left_string (3) == String ("no-"))
    {
      var = ly_symbol2scm (varstr.nomid_string (0, 3).to_str0 ());
      val = scm_from_bool (!to_boolean (val));
    }

  SCM handle = scm_hashq_get_handle (option_hash_, var);
  if (handle == SCM_BOOL_F)
    warning (_f ("no such internal option: %s", varstr.to_str0 ()));

  internal_set_option (var, val);
  return SCM_UNSPECIFIED;
}

LY_DEFINE (ly_get_option, "ly:get-option", 1, 0, 0, (SCM var),
	   "Get a global option setting.")
{
  SCM_ASSERT_TYPE (scm_is_symbol (var), var,
		   SCM_ARG1, __FUNCTION__, "symbol");
  return scm_hashq_ref (option_hash_, var, SCM_BOOL_F);
}
