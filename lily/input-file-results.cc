/*
  scores.cc -- implement some globals

  source file of the GNU LilyPond music typesetter

  (c)  1997--2003 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/
#include "config.h"

#include <errno.h>
#include <sys/types.h>
#include <stdio.h>


#if HAVE_SYS_STAT_H 
#include <sys/stat.h>
#endif
#include <unistd.h>


#include "main.hh"
#include "score.hh"
#include "string.hh"
#include "paper-def.hh"
#include "warn.hh"
#include "parray.hh"
#include "file-path.hh"
#include "input-file-results.hh"
#include "my-lily-parser.hh"
#include "source.hh"
#include "lily-version.hh"
#include "scm-hash.hh"
#include "ly-modules.hh"

bool store_locations_global_b;


/*
  no ! suffix since it doesn't modify 1st argument.
 */
LY_DEFINE(ly_set_point_and_click, "ly:set-point-and-click", 1, 0, 0,
	  (SCM what),
	  "Set the options for Point-and-click source specials output. The\n"
"argument is a symbol.  Possible options are @code{none} (no source specials),\n"
"@code{line} and @code{line-column}")
{
  /*
    UGH.
   */
  SCM val = SCM_BOOL_F;
  if (ly_symbol2scm ("line-column") == what)
    val = gh_eval_str ("line-column-location");
  else if (what == ly_symbol2scm ("line"))
    val = gh_eval_str ("line-location");

  extern SCM lily_module; 
  scm_module_define (lily_module, ly_symbol2scm ("point-and-click"), val);

  store_locations_global_b =gh_procedure_p (val);
  return SCM_UNSPECIFIED;
}

void
write_dependency_file (String fn,
		       Array<String> targets,
		       Array<String> deps)
{
  const int WRAPWIDTH = 65;

  progress_indication (_f ("dependencies output to `%s'...", fn.to_str0 ()));
  progress_indication ("\n");
  FILE * f = fopen  (fn.to_str0 (), "w");
  if (!f)
    warning (_f ("can't open file: `%s'", fn));

  fprintf (f, "# Generated automatically by: %s\n", gnu_lilypond_version_string ().to_str0 ());
  
  String out;
  for (int i=0; i < targets.size (); i ++)
     out += dependency_prefix_global + targets[i] + " ";
  out +=  ": ";

  for (int i=0; i < deps.size (); i ++)
    {
      if (out.length () > WRAPWIDTH)
	{
	  fprintf (f, "%s\\\n", out.to_str0 ());
	  out = "  ";
	}
      String dep = deps[i];
      if (!dependency_prefix_global.is_empty ())
	{
	  if (dep.index ('/') < 0)
	    dep = dependency_prefix_global + dep;
	}
      out  += " " +  dep;
    }
  fprintf (f, "%s\n",  out.to_str0 ());
}


void
Input_file_results::do_deps (String output)
{
  if (dependency_global_b)
    {
      Path p = split_path (output);
      p.ext = "dep";
      write_dependency_file (p.to_string (),
			     target_strings_,
			     inclusion_names_);
    }
}

Input_file_results::~Input_file_results ()
{
  inclusion_names_.clear ();
  if (header_)
    header_ = SCM_EOL;

  global_input_file =0;

  ly_clear_anonymous_modules();
}



Input_file_results* global_input_file;

Input_file_results::Input_file_results (String init, String in_file, String out_file)
{
  header_ = ly_make_anonymous_module ();
  global_input_file = this;
  score_count_ = 0;
  sources_.set_path (&global_path);
  

  progress_indication (_f ("Now processing: `%s'", in_file.to_str0 ()));
  progress_indication ("\n");

  My_lily_parser parser (this);
  parser.parse_file (init, in_file, out_file);
  
  if (parser.error_level_)
    {
      exit_status_global  = 1;
    }

  
  do_deps (out_file);
}


void
do_one_file (String init, String in_file, String out_file) 
{
  if (init.length () && global_path.find (init).is_empty ())
    {
      warning (_f ("can't find file: `%s'", init));
      warning (_f ("(search path: `%s')", global_path.to_string ().to_str0 ()));
      return;
    }

  if ((in_file != "-") && global_path.find (in_file).is_empty ())
    {
      warning (_f ("can't find file: `%s'", in_file));
      return;
    }

  Input_file_results inp_file (init, in_file, out_file);
}
