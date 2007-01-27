/*
  font-config-scheme.cc -- implement FontConfig bindings.

  source file of the GNU LilyPond music typesetter

  (c) 2005--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>

*/

#include "lily-guile.hh"
#include "string-convert.hh"
#include "warn.hh"

#include <fontconfig/fontconfig.h>

string
display_fontset (FcFontSet *fs)
{
  string retval;
  
  int j;
  for (j = 0; j < fs->nfont; j++)
    {
      FcChar8 *font;
      FcChar8 *str;

      font = FcNameUnparse (fs->fonts[j]);
      if (FcPatternGetString (fs->fonts[j], FC_FILE, 0, &str) == FcResultMatch)
	retval += String_convert::form_string ("FILE %s\n", str);
      if (FcPatternGetString (fs->fonts[j], FC_FAMILY, 0, &str) == FcResultMatch)
	retval += String_convert::form_string ("family %s\n ", str);
      if (FcPatternGetString (fs->fonts[j],
			      "designsize", 0, &str) == FcResultMatch)
	retval += String_convert::form_string ("designsize %s\n ", str);
      
      retval += String_convert::form_string ("%s\n", (const char*) font);
      free (font);
    }
  
  return retval;
}

string
display_strlist (char const*what, FcStrList *slist)
{
  string retval;
  while (FcChar8 *dir = FcStrListNext (slist))
    {
      retval += String_convert::form_string ("%s: %s\n", what, dir);
    }
  return retval;
}

string
display_config (FcConfig *fcc)
{
  string retval;
  retval += display_strlist ("Config files", FcConfigGetConfigFiles(fcc));
  retval +=  display_strlist ("Config dir", FcConfigGetConfigDirs(fcc));
  retval +=  display_strlist ("Font dir", FcConfigGetFontDirs(fcc));
  return retval;
}

string
display_list (FcConfig *fcc)
{
  FcPattern*pat = FcPatternCreate ();

  FcObjectSet *os = 0;
  if (!os)
    os = FcObjectSetBuild (FC_FAMILY, FC_STYLE, (char *) 0);

  FcFontSet *fs = FcFontList (fcc, pat, os);
  FcObjectSetDestroy (os);
  if (pat)
    FcPatternDestroy (pat);

  string retval;
  if (fs)
    {
      retval = display_fontset (fs);
      FcFontSetDestroy (fs);
    }
  return retval;
}


LY_DEFINE (ly_font_config_get_font_file, "ly:font-config-get-font-file", 1, 0, 0,
	   (SCM name),
	   "Get the file for font @var{name}")
{
  LY_ASSERT_TYPE (scm_is_string, name, 1);
  
  FcPattern*pat = FcPatternCreate ();
  FcValue val;
  
  val.type = FcTypeString;
  val.u.s = (const FcChar8*)ly_scm2string (name).c_str (); // FC_SLANT_ITALIC;
  FcPatternAdd(pat, FC_FAMILY, val, FcFalse);

  FcResult result;
  SCM scm_result = SCM_BOOL_F;

  FcConfigSubstitute (NULL, pat, FcMatchFont);
  FcDefaultSubstitute (pat);
  
  pat = FcFontMatch(NULL, pat, &result);
  FcChar8 *str = 0;
  if (FcPatternGetString (pat, FC_FILE, 0, &str) == FcResultMatch)
    scm_result = scm_from_locale_string ((char const*) str);

  FcPatternDestroy (pat);

  return scm_result;
}
	   
LY_DEFINE (ly_font_config_display_fonts, "ly:font-config-display-fonts", 0, 0, 0,
	   (),
	   "Dump a list of all fonts visible to FontConfig.")
{
  string str = display_list (NULL);
  str += display_config (NULL);

  progress_indication (str);
  
  return SCM_UNSPECIFIED;
}


