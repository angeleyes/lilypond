/*   
  kpath.cc -- glue kpathsea to lily. Need some ugly kludges for gcc 2.96

  source file of the GNU LilyPond music typesetter

  (c) 2000--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "kpath.hh"

#include <cstdio>
#include <cstring>

/*

The problem, as far as I can tell, is that MacOS X has its getopt
prototype in <unistd.h>, while I think other operating systems have it
in other places. <unistd.h> is included by kpathsea.h, so you end up
renaming both conflicting prototypes to YAKLUDGE.

I found a somewhat more elegant patch for this: Just #include
<unistd.h> before defining YAKLUDGE.

*/

#include <unistd.h>	

#include "config.hh"

#define popen REALLYUGLYKLUDGE
#define pclose ANOTHERREALLYUGLYKLUDGE
#define getopt YAKLUDGE

#if HAVE_KPATHSEA_KPATHSEA_H
extern "C" {
#include <kpathsea/kpathsea.h>
#include <kpathsea/tex-file.h>
}
#endif

#include "file-path.hh"
#include "main.hh"
#include "source-file.hh"
#include "warn.hh"

String
kpathsea_find_afm (char const *name)
{
#if (KPATHSEA && HAVE_KPSE_FIND_FILE)

  if (char *afm = kpse_find_file (name, kpse_afm_format, false))
    return afm;
#if 0 /* Do not mutter about afms, since we try to find them first, and
	 lots of TFMs don't have AFMs. */
  warning (_f ("kpathsea can not find AFM file `%s'", name));
#endif
#endif
  return "";
}

String
kpathsea_find_tfm (char const *name)
{
  String file_name = global_path.find (String (name) + ".tfm");
#if (KPATHSEA && HAVE_KPSE_FIND_FILE)
  if (file_name.is_empty ())
    {
      /* If invoked for a TeX font, we could do TRUE (must exist).
	 We could also do:
	   p = kpse_find_file (name, kpse_mf_format, false);
	   if (p)
	     p = kpse_find_file (name, kpse_mf_format, true);

	     but we assume that if there is a .PFA, there is also a .TFM,
	 and it's no use generating TFMs on the fly, because PFAs cannot
	 be generated on the fly. */
      if (char *tfm = kpse_find_file (name, kpse_tfm_format, false))
	return tfm;
      warning (_f ("kpathsea can not find TFM file: `%s'", name));
    }
#endif
  return file_name;
}

#if KPATHSEA
/* FIXME: this should be part of kpathsea */

static kpse_file_format_type
kpathsea_find_format (String name)
{
  for (int i = 0; i < kpse_last_format; i++)
    {
      if (!kpse_format_info[i].type)
        kpse_init_format ((kpse_file_format_type) i);

      char const **suffixes[] = { kpse_format_info[i].suffix,
				  kpse_format_info[i].alt_suffix };
      for (int j = 0; j < 2; j++)
	for (char const **p = suffixes[j]; p && *p; p++)
	  {
	    String suffix = *p;
	    if (name.right_string (suffix.length ()) == suffix)
	      return (kpse_file_format_type) i;
	  }
    }
  return kpse_last_format;
}
#endif

String
kpathsea_gulp_file_to_string (String name)
{
  String file_name = global_path.find (name);

#if (KPATHSEA && HAVE_KPSE_FIND_FILE)
  if (file_name.is_empty ())
    {
      char *p = kpse_find_file (name.to_str0 (), kpathsea_find_format (name),
	true);
      if (p)
	file_name = p;
      else
	warning (_f ("kpathsea can not find file: `%s'", name));
    }
#endif

  if (file_name.is_empty ())
    error (_f ("can't find file: `%s'", name));

  if (verbose_global_b)
    progress_indication ("[" + file_name);

  int filesize;
  char *str = gulp_file (file_name, &filesize);
  String string (str);
  delete[] str;
  
  if (verbose_global_b)
    progress_indication ("]");

  return string;
}


LY_DEFINE (ly_find_file, "ly:find-file",
	   1, 0, 0, (SCM name),
	   "Return the absolute file name of @var{name},"
	   "or @code{#f} if not found.")
{
  SCM_ASSERT_TYPE (scm_is_string (name), name, SCM_ARG1, __FUNCTION__, "string");

  String nm = ly_scm2string (name);
  String file_name = global_path.find (nm);
  if (file_name.is_empty ())
    return SCM_BOOL_F;
  
  return scm_makfrom0str (file_name.to_str0 ());
}

LY_DEFINE (ly_kpathsea_find_file, "ly:kpathsea-find-file",
	   1, 0, 0, (SCM name),
	   "Return the absolute file name of @var{name},"
	   "or @code{#f} if not found.")
{
  SCM_ASSERT_TYPE (scm_is_string (name), name, SCM_ARG1, __FUNCTION__, "string");

  String nm = ly_scm2string (name);
  String file_name = global_path.find (nm);
  if (file_name.is_empty ())
    {
      if (char *p = kpse_find_file (nm.to_str0 (), kpathsea_find_format (nm),
				    true))
	return scm_makfrom0str (p);
      return SCM_BOOL_F;
    }
  return scm_makfrom0str (file_name.to_str0 ());
}

LY_DEFINE (ly_kpathsea_expand_variable, "ly:kpathsea-expand-variable",
	   1, 0, 0, (SCM var),
	   "Return the expanded version  @var{var}.")
{
  SCM_ASSERT_TYPE (scm_is_string (var), var, SCM_ARG1, __FUNCTION__, "string");

  String nm = ly_scm2string (var);
  char *result =  kpse_var_expand (nm.to_str0 ());
  SCM ret =  scm_makfrom0str (result);
  delete[] result;

  return ret;
}

void
initialize_kpathsea (char *av0)
{
#if KPATHSEA && HAVE_KPATHSEA_KPATHSEA_H

  /*
   initialize kpathsea
   */
  kpse_set_program_name (av0, NULL);
  kpse_maketex_option ("tfm", TRUE);
#endif
}
