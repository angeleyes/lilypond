/*
  main.cc -- implement main () entrypoint.

  source file of the GNU LilyPond music typesetter

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "main.hh"

#include <cassert>
#include <clocale>
#include <cstring>
#include <unistd.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "config.hh"

#if HAVE_GETTEXT
#include <libintl.h>
#endif

#include "all-font-metrics.hh"
#include "file-name.hh"
#include "freetype.hh"
#include "getopt-long.hh"
#include "global-ctor.hh"
#include "lily-guile.hh"
#include "lily-version.hh"
#include "misc.hh"
#include "output-def.hh"
#include "string-convert.hh"
#include "version.hh"
#include "warn.hh"

/*
 * Global options that can be overridden through command line.
 */

/* Names of header fields to be dumped to a separate file. */
Array<String> dump_header_fieldnames_global;

/* Name of initialisation file. */
String init_name_global;

/* Selected output format.
   One of tex, ps, scm, as.
*/
String output_backend_global = "ps";
String output_format_global = "pdf";

bool is_pango_format_global;
bool is_TeX_format_global;

/* Current output name. */
String output_name_global;

/* Run in safe mode? */
bool be_safe_global = false;

/* Provide URI links to the original file */
bool point_and_click_global = true;

/* Verbose progress indication? */
bool be_verbose_global = false;

/* Scheme code to execute before parsing, after .scm init
   This is where -e arguments are appended to.
*/
String init_scheme_code_string = "(begin #t ";

bool make_preview = false;
bool make_pages = true;

/*
 * Miscellaneous global stuff.
 */
File_path global_path;

/*
 * File globals.
 */

static char const *AUTHORS
= "  Han-Wen Nienhuys <hanwen@cs.uu.nl>\n"
  "  Jan Nieuwenhuizen <janneke@gnu.org>\n";

static char const *PROGRAM_NAME = "lilypond";
static char const *PROGRAM_URL = "http://lilypond.org";

static char const *NOTICE
= _i ("This program is free software.  It is covered by the GNU General Public\n"
      "License and you are welcome to change it and/or distribute copies of it\n"
      "under certain conditions.  Invoke as `%s --warranty' for more\n"
      "information.\n");

static char const *WARRANTY
= _i ("    This program is free software; you can redistribute it and/or\n"
      "modify it under the terms of the GNU General Public License version 2\n"
      "as published by the Free Software Foundation.\n"
      "\n"
      "    This program is distributed in the hope that it will be useful,\n"
      "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
      "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n"
      "General Public License for more details.\n"
      "\n"
      "    You should have received a copy (refer to the file COPYING) of the\n"
      "GNU General Public License along with this program; if not, write to\n"
      "the Free Software Foundation, Inc., 59 Temple Place - Suite 330,\n"
      "Boston, MA 02111-1307, USA.\n");

/* Where the init files live.  Typically:
   LILYPOND_DATADIR = /usr/share/lilypond
*/
String prefix_directory;

/* The jail specification: USER,GROUP,JAIL,DIR. */
String jail_spec;

/*  The option parser */
static Getopt_long *option_parser = 0;

/* Internationalisation kludge in two steps:
 * use _i () to get entry in POT file
 * call gettext () explicitely for actual "translation"  */

static Long_option_init options_static[]
= {
  {_i ("EXT"), "backend", 'b', _i ("select backend to use")},
  {_i ("EXPR"), "evaluate", 'e',
   _i ("set option, use -e '(ly:option-usage)' for help")},
  /* Bug in option parser: --output =foe is taken as an abbreviation
     for --output-format.  */
  {_i ("EXTs"), "formats", 'f', _i ("list of formats to dump")},
  {0, "help", 'h',  _i ("print this help")},
  {_i ("FIELD"), "header", 'H',  _i ("write header field to BASENAME.FIELD")},
  {_i ("DIR"), "include", 'I',  _i ("add DIR to search path")},
  {_i ("FILE"), "init", 'i',  _i ("use FILE as init file")},
  {_i ("FILE"), "output", 'o',  _i ("write output to FILE (suffix will be added)")},
  {_i ("USER,GROUP,JAIL,DIR"), "jail", 'j', _i ("chroot to JAIL, become USER:GROUP and cd into DIR")},
  {0, "preview", 'p',  _i ("generate a preview")},
  {0, "no-pages", 0, _i ("don't generate full pages")},
  {0, "png", 0, _i ("generate PNG")},
  {0, "ps", 0, _i ("generate PostScript")},
  {0, "dvi", 0, _i ("generate DVI")},
  {0, "pdf", 0, _i ("generate PDF (default)")},
  {0, "tex", 0, _i ("generate TeX")},
  {0, "safe-mode", 's',  _i ("run in safe mode")},
  {0, "version", 'v',  _i ("print version number")},
  {0, "verbose", 'V', _i ("be verbose")},
  {0, "warranty", 'w',  _i ("show warranty and copyright")},
  {0, 0, 0, 0}
};

static void
dir_info (FILE *out)
{
  fputs ("\n", out);
  fprintf (out, "LILYPOND_DATADIR=\"%s\"\n", LILYPOND_DATADIR);
  if (char const *env = getenv ("LILYPONDPREFIX"))
    fprintf (out, "LILYPONDPREFIX=\"%s\"\n", env);
  fprintf (out, "LOCALEDIR=\"%s\"\n", LOCALEDIR);

  fprintf (out, "\nEffective prefix: \"%s\"\n", prefix_directory.to_str0 ());
}

static void
copyright ()
{
  printf (_f ("Copyright (c) %s by\n%s  and others.",
	      "1996--2005",
	      AUTHORS).to_str0 ());
  printf ("\n");
}

static void
identify (FILE *out)
{
  fputs (gnu_lilypond_version_string ().to_str0 (), out);
  fputs ("\n", out);
}

static void
notice ()
{
  identify (stdout);
  puts (_f (NOTICE, PROGRAM_NAME).to_str0 ());
  printf ("\n");
  copyright ();
}

static void
usage ()
{
  /* No version number or newline here.  It confuses help2man.  */
  printf (_f ("Usage: %s [OPTION]... FILE...", PROGRAM_NAME).to_str0 ());
  printf ("\n\n");
  printf (_ ("Typeset music and/or produce MIDI from FILE.").to_str0 ());
  printf ("\n\n");
  printf (_ ("LilyPond produces beautiful music notation.").to_str0 ());
  printf ("\n");
  printf (_f ("For more information, see %s", PROGRAM_URL).to_str0 ());
  printf ("\n\n");
  printf (_ ("Options:").to_str0 ());
  printf ("\n");
  printf (Long_option_init::table_string (options_static).to_str0 ());
  printf ("\n");
  printf (_f ("Report bugs to %s.", "bug-lilypond@gnu.org").to_str0 ());
  printf ("\n");
  printf ("\n");
}

static void
warranty ()
{
  identify (stdout);
  printf ("\n");
  copyright ();
  printf ("\n");
  printf (_ (WARRANTY).to_str0 ());
}

static void
setup_paths ()
{
  prefix_directory = LILYPOND_DATADIR;
  if (char const *env = getenv ("LILYPONDPREFIX"))
    prefix_directory = env;

  global_path.append ("");

  /* Adding mf/out make lilypond unchanged source directory, when setting
     LILYPONDPREFIX to lilypond-x.y.z */
  char *suffixes[] = {"ly", "ps", "scm", 0 };

  Array<String> dirs;
  for (char **s = suffixes; *s; s++)
    {
      String path = prefix_directory + to_string ('/') + String (*s);
      dirs.push (path);
    }

  /*
    ugh. C&P font-config.cc
  */
  struct stat statbuf; 
  String builddir = prefix_directory + "/mf/out/";
  if (stat (builddir.to_str0 (), &statbuf) == 0)
    {
      dirs.push (builddir.to_str0 ());
    }
  else
    {
      dirs.push (prefix_directory + "/fonts/otf/");
      dirs.push (prefix_directory + "/fonts/type1/");
      dirs.push (prefix_directory + "/fonts/cff/");
      dirs.push (prefix_directory + "/fonts/svg/");
      dirs.push (prefix_directory + "/fonts/cff/");
    }

  for (int i = 0; i < dirs.size (); i++)
    global_path.prepend (dirs[i]);  
}
  
static void
prepend_load_path (String dir)
{
  String s = "(set! %load-path (cons \"" + dir + "\" %load-path))";
  scm_c_eval_string (s.to_str0 ());
}

void init_global_tweak_registry ();
void init_fontconfig ();

static void
do_chroot_jail ()
{
  /* Now we chroot, setuid/setgrp and chdir.  If something goes wrong,
     we exit (this is a security-sensitive area).  First we split
     jail_spec into its components, then we retrieve the user/group id
     (necessarily *before* chroot'ing) and finally we perform the
     actual actions.  */

  enum Jail
    {
      USER_NAME, GROUP_NAME, JAIL, DIR, JAIL_MAX
    };

  Array<String> components = String_convert::split (jail_spec, ',');
  if (components.size () != JAIL_MAX)
    {
      error (_f ("expected %d arguments with jail, found: %d", JAIL_MAX,
		 components.size ()));
      exit (2);
    }

  /* Hmm.  */
  errno = 0;

  int uid;
  if (passwd * passwd = getpwnam (components[USER_NAME].to_str0 ()))
    uid = passwd->pw_uid;
  else
    {
      if (errno == 0)
	error (_f ("no such user: %s", components[USER_NAME]));
      else
	error (_f ("can't get user id from user name: %s: %s",
		   components[USER_NAME],
		   strerror (errno)));
      exit (3);
    }

  /* Hmm.  */
  errno = 0;

  int gid;
  if (group * group = getgrnam (components[GROUP_NAME].to_str0 ()))
    gid = group->gr_gid;
  else
    {
      if (errno == 0)
	error (_f ("no such group: %s", components[GROUP_NAME]));
      else
	error (_f ("can't get group id from group name: %s: ",
		   components[GROUP_NAME],
		   strerror (errno)));
      exit (3);
    }

  if (chroot (components[JAIL].to_str0 ()))
    {
      error (_f ("can't chroot to: %s: %s", components[JAIL],
		 strerror (errno)));
      exit (3);
    }

  if (setgid (gid))
    {
      error (_f ("can't change group id to: %d: %s", gid, strerror (errno)));
      exit (3);
    }

  if (setuid (uid))
    {
      error (_f ("can't change user id to: %d: %s", uid, strerror (errno)));
      exit (3);
    }

  if (chdir (components[DIR].to_str0 ()))
    {
      error (_f ("can't change working directory to: %s: %s", components[DIR],
		 strerror (errno)));
      exit (3);
    }
}

void test_pango ();

static void
main_with_guile (void *, int, char **)
{
  /* Engravers use lily.scm contents, need to make Guile find it.
     Prepend onto GUILE %load-path, very ugh. */

  prepend_load_path (prefix_directory);
  prepend_load_path (prefix_directory + "/scm");

  if (be_verbose_global)
    dir_info (stderr);
  is_TeX_format_global = (output_backend_global == "tex"
			  || output_backend_global == "texstr");

  is_pango_format_global = !is_TeX_format_global;

  ly_c_init_guile ();
  call_constructors ();
  init_global_tweak_registry ();
  init_fontconfig ();

  init_freetype ();

  all_fonts_global = new All_font_metrics (global_path.to_string ());

  init_scheme_code_string += ")";
  scm_c_eval_string ((char *) init_scheme_code_string.to_str0 ());

  /* We accept multiple independent music files on the command line to
     reduce compile time when processing lots of small files.
     Starting the GUILE engine is very time consuming.  */

  SCM files = SCM_EOL;
  SCM *tail = &files;
  while (char const *arg = option_parser->get_next_arg ())
    {
      *tail = scm_cons (scm_makfrom0str (arg), SCM_EOL);
      tail = SCM_CDRLOC (*tail);
    }
  delete option_parser;
  option_parser = 0;

  if (files == SCM_EOL)
    {
      /* No FILE arguments is now a usage error to help newbies.  If you
	 want a filter, you're not a newbie and should know to use file
	 argument `-'.  */
      usage ();
      exit (2);
    }

  if (! jail_spec.is_empty ())
    do_chroot_jail ();

  SCM result = scm_call_1 (ly_lily_module_constant ("lilypond-main"), files);
  (void) result;

  /* Unreachable.  */
  exit (0);
}

static void
setup_localisation ()
{
#if HAVE_GETTEXT
  /* Enable locales */
  setlocale (LC_ALL, "");

  /* FIXME: check if this is still true.
     Disable localisation of float values.  This breaks TeX output.  */
  setlocale (LC_NUMERIC, "C");

  String name (PACKAGE);
  name.to_lower ();
  bindtextdomain (name.to_str0 (), LOCALEDIR);
  textdomain (name.to_str0 ());
#endif
}

static void
add_output_format (String format)
{
  if (output_format_global != "")
    output_format_global += ",";
  output_format_global += format;
}

static void
parse_argv (int argc, char **argv)
{
  bool show_help = false;
  option_parser = new Getopt_long (argc, argv, options_static);
  while (Long_option_init const *opt = (*option_parser) ())
    {
      switch (opt->shortname_char_)
	{
	case 0:
	  if (String (opt->longname_str0_) == "png"
	      || String (opt->longname_str0_) == "pdf"
	      || String (opt->longname_str0_) == "ps"
	      || String (opt->longname_str0_) == "dvi"
	      || String (opt->longname_str0_) == "tex")
	    {
	      add_output_format (opt->longname_str0_);
	    }
	  else if (String (opt->longname_str0_) == "preview")
	    make_preview = true;
	  else if (String (opt->longname_str0_) == "no-pages")
	    make_pages = false;
	  break;

	case 'v':
	  notice ();
	  exit (0);
	  break;
	case 'o':
	  {
	    String s = option_parser->optional_argument_str0_;
	    File_name file_name (s);
	    output_name_global = file_name.to_string ();
	  }
	  break;
	case 'j':
	  jail_spec = option_parser->optional_argument_str0_;
	  break;
	case 'e':
	  init_scheme_code_string += option_parser->optional_argument_str0_;
	  break;
	case 'w':
	  warranty ();
	  exit (0);
	  break;
	  
	case 'b':
	  output_backend_global = option_parser->optional_argument_str0_;
	  break;

	case 'f':
	  output_format_global = option_parser->optional_argument_str0_;
	  break;
	  
	case 'H':
	  dump_header_fieldnames_global
	    .push (option_parser->optional_argument_str0_);
	  break;
	case 'I':
	  global_path.append (option_parser->optional_argument_str0_);
	  break;
	case 'i':
	  init_name_global = option_parser->optional_argument_str0_;
	  break;
	case 'h':
	  show_help = true;
	  break;
	case 'V':
	  be_verbose_global = true;
	  break;
	case 's':
	  be_safe_global = true;
	  break;
	case 'p':
	  make_preview = true;
	  break;
	default:
	  programming_error (to_string ("unhandled short option: %c",
					opt->shortname_char_));
	  assert (false);
	  break;
	}
    }

  if (show_help)
    {
      identify (stdout);
      usage ();
      if (be_verbose_global)
	dir_info (stdout);
      exit (0);
    }
}

int
main (int argc, char **argv)
{
  setup_localisation ();
  setup_paths ();
  parse_argv (argc, argv);
  identify (stderr);

  scm_boot_guile (argc, argv, main_with_guile, 0);

  /* Unreachable */
  return 0;
}
