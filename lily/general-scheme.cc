/*
  lily-guile.cc -- implement assorted Guile bindings

  source file of the GNU LilyPond music typesetter

  (c) 1998--2005 Jan Nieuwenhuizen <janneke@gnu.org>
  Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "config.hh"

#include <libintl.h>  /* gettext on MacOS X */
#include <math.h>  /* isinf */
#include <stdio.h>
#include <string.h>  /* memset */
#if HAVE_UTF8_WCHAR_H
#include <utf8/wchar.h>  /* wcrtomb */
#else
#include <wchar.h> /* wcrtomb */
#endif

#include "libc-extension.hh"
#include "lily-guile.hh"
#include "string.hh"
#include "misc.hh"
#include "warn.hh"
#include "version.hh"
#include "dimensions.hh"
#include "main.hh"
#include "file-path.hh"

/* MacOS S fix:
   source-file.hh includes cmath which undefines isinf and isnan
*/
#ifdef __APPLE__
inline int my_isinf (Real r) { return isinf (r); }
inline int my_isnan (Real r) { return isnan (r); }
#endif

LY_DEFINE (ly_find_file, "ly:find-file",
	   1, 0, 0, (SCM name),
	   "Return the absolute file name of @var{name}, "
	   "or @code{#f} if not found.")
{
  SCM_ASSERT_TYPE (scm_is_string (name), name, SCM_ARG1, __FUNCTION__, "string");

  String nm = ly_scm2string (name);
  String file_name = global_path.find (nm);
  if (file_name.is_empty ())
    return SCM_BOOL_F;

  return scm_makfrom0str (file_name.to_str0 ());
}

/*
  Ugh. Gulped file is copied twice. (maybe thrice if you count stdio
  buffering.)
*/
LY_DEFINE (ly_gulp_file, "ly:gulp-file",
	   1, 0, 0, (SCM name),
	   "Read the file @var{name}, and return its contents in a string.  "
	   "The file is looked up using the search path.")
{
  SCM_ASSERT_TYPE (scm_is_string (name), name, SCM_ARG1, __FUNCTION__, "string");
  String contents = gulp_file_to_string (ly_scm2string (name), true);
  return scm_from_locale_stringn (contents.get_str0 (), contents.length ());
}

LY_DEFINE (ly_error, "ly:error",
	   1, 0, 1, (SCM str, SCM rest),
	   "Scheme callable function to issue the error @code{msg}. "
	   "The error is formatted with @code{format} and @code{rest}.")
{
  SCM_ASSERT_TYPE (scm_is_string (str), str, SCM_ARG1, __FUNCTION__, "string");
  str = scm_simple_format (SCM_BOOL_F, str, rest);
  error (ly_scm2string (str));
  return SCM_UNSPECIFIED;
}

LY_DEFINE (ly_message, "ly:message",
	   1, 0, 1, (SCM str, SCM rest),
	   "Scheme callable function to issue the message @code{msg}. "
	   "The message is formatted with @code{format} and @code{rest}.")
{
  SCM_ASSERT_TYPE (scm_is_string (str), str, SCM_ARG1, __FUNCTION__, "string");
  str = scm_simple_format (SCM_BOOL_F, str, rest);
  message (ly_scm2string (str));
  return SCM_UNSPECIFIED;
}

LY_DEFINE (ly_progress, "ly:progress",
	   1, 0, 1, (SCM str, SCM rest),
	   "Scheme callable function to print progress @code{str}. "
	   "The message is formatted with @code{format} and @code{rest}.")
{
  SCM_ASSERT_TYPE (scm_is_string (str), str, SCM_ARG1, __FUNCTION__, "string");
  str = scm_simple_format (SCM_BOOL_F, str, rest);
  progress_indication (ly_scm2string (str));
  return SCM_UNSPECIFIED;
}

LY_DEFINE (ly_programming_error, "ly:programming-error",
	   1, 0, 1, (SCM str, SCM rest),
	   "Scheme callable function to issue the warning @code{msg}. "
	   "The message is formatted with @code{format} and @code{rest}.")
{
  SCM_ASSERT_TYPE (scm_is_string (str), str, SCM_ARG1, __FUNCTION__, "string");
  str = scm_simple_format (SCM_BOOL_F, str, rest);
  programming_error (ly_scm2string (str));
  return SCM_UNSPECIFIED;
}

LY_DEFINE (ly_warning, "ly:warning",
	   1, 0, 1, (SCM str, SCM rest),
	   "Scheme callable function to issue the warning @code{str}. "
	   "The message is formatted with @code{format} and @code{rest}.")
{
  SCM_ASSERT_TYPE (scm_is_string (str), str, SCM_ARG1, __FUNCTION__, "string");
  str = scm_simple_format (SCM_BOOL_F, str, rest);
  warning (ly_scm2string (str));
  return SCM_UNSPECIFIED;
}

LY_DEFINE (ly_dir_p, "ly:dir?",
	   1, 0, 0, (SCM s),
	   "type predicate. A direction is @code{-1}, @code{0} or "
	   "@code{1}, where @code{-1} represents "
	   "left or down and @code{1} represents right or up.")
{
  if (scm_is_number (s))
    {
      int i = scm_to_int (s);
      return (i >= -1 && i <= 1) ? SCM_BOOL_T : SCM_BOOL_F;
    }
  return SCM_BOOL_F;
}

LY_DEFINE (ly_assoc_get, "ly:assoc-get",
	   2, 1, 0,
	   (SCM key, SCM alist, SCM default_value),
	   "Return value if KEY in ALIST, else DEFAULT-VALUE "
	   "(or #f if not specified).")
{
  SCM handle = scm_assoc (key, alist);

  if (default_value == SCM_UNDEFINED)
    default_value = SCM_BOOL_F;

  if (scm_is_pair (handle))
    return scm_cdr (handle);
  else
    return default_value;
}

LY_DEFINE (ly_number2string, "ly:number->string",
	   1, 0, 0, (SCM s),
	   "Convert @var{num} to a string without generating many decimals.")
{
  SCM_ASSERT_TYPE (scm_is_number (s), s, SCM_ARG1, __FUNCTION__, "number");

  char str[400];			// ugh.

  if (scm_exact_p (s) == SCM_BOOL_F)
    {
      Real r (scm_to_double (s));
#ifdef __APPLE__
      if (my_isinf (r) || my_isnan (r))
#else
	if (isinf (r) || isnan (r))
#endif
	  {
	    programming_error (_ ("infinity or NaN encountered while converting Real number"));
	    programming_error (_ ("setting to zero"));
			       
	    r = 0.0;
	  }

      snprintf (str, sizeof (str), "%08.4f", r);
    }
  else
    snprintf (str, sizeof (str), "%d", scm_to_int (s));

  return scm_makfrom0str (str);
}

LY_DEFINE (ly_version, "ly:version", 0, 0, 0, (),
	   "Return the current lilypond version as a list, e.g. @code{(1 3 127 uu1)}. ")
{
  char const *vs = "\'(" MAJOR_VERSION " " MINOR_VERSION " " PATCH_LEVEL " " MY_PATCH_LEVEL ")";

  return scm_c_eval_string ((char *)vs);
}

LY_DEFINE (ly_unit, "ly:unit", 0, 0, 0, (),
	   "Return the unit used for lengths as a string.")
{
  return scm_makfrom0str (INTERNAL_UNIT);
}

LY_DEFINE (ly_dimension_p, "ly:dimension?", 1, 0, 0, (SCM d),
	   "Return @var{d} is a number. Used to distinguish length "
	   "variables from normal numbers.")
{
  return scm_number_p (d);
}

/*
  Debugging mem leaks:
*/
LY_DEFINE (ly_protects, "ly:protects",
	   0, 0, 0, (),
	   "Return hash of protected objects.")
{
  return scm_protects;
}

LY_DEFINE (ly_gettext, "ly:gettext",
	   1, 0, 0, (SCM string),
	   "Gettext wrapper.")
{
  SCM_ASSERT_TYPE (scm_is_string (string), string, SCM_ARG1,
		   __FUNCTION__, "string");
  return scm_makfrom0str (_ (scm_i_string_chars (string)).to_str0 ());
}

LY_DEFINE (ly_output_backend, "ly:output-backend",
	   0, 0, 0, (),
	   "Return name of output backend.")
{
  return scm_makfrom0str (output_backend_global.to_str0 ());
}

LY_DEFINE (ly_output_formats, "ly:output-formats",
	   0, 0, 0, (),
	   "Formats passed to --format as a list of strings, "
	   "used for the output.")
{
  Array<String> output_formats = split_string (output_format_global, ',');

  SCM lst = SCM_EOL;
  int output_formats_count = output_formats.size ();
  for (int i = 0; i < output_formats_count; i ++)
    lst = scm_cons (scm_makfrom0str (output_formats[i].to_str0 ()), lst);

  return lst;
}

LY_DEFINE (ly_wchar_to_utf_8, "ly:wide-char->utf-8",
	   1, 0, 0, (SCM wc),
	   "Encode the Unicode codepoint @var{wc} as UTF-8")
{
  char buf[100];

  SCM_ASSERT_TYPE (scm_is_integer (wc), wc, SCM_ARG1, __FUNCTION__, "integer");
  wchar_t wide_char = (wchar_t) scm_to_int (wc);

  mbstate_t state;
  memset (&state, '\0', sizeof (state));
  memset (buf, '\0', sizeof (buf));

  wcrtomb (buf, wide_char, &state);
  
  return scm_makfrom0str (buf);
}
	  
LY_DEFINE (ly_effective_prefix, "ly:effective-prefix",
	   0, 0, 0, (),
	   "Return effective prefix.")
{
  return scm_makfrom0str (prefix_directory.to_str0 ());
}


