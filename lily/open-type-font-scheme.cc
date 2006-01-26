/*
  open-type-font.cc -- implement Open_type_font

  source file of the GNU LilyPond music typesetter

  (c) 2004--2006 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "modified-font-metric.hh"
#include "open-type-font.hh"

LY_DEFINE (ly_font_sub_fonts, "ly:font-sub-fonts", 1, 0, 0,
	   (SCM font),
	   "Given the font metric @var{font} of an OpenType font, return the "
	   "names of the subfonts within @var{font}.")
{
  Font_metric *fm = unsmob_metrics (font);
  SCM_ASSERT_TYPE (fm, font, SCM_ARG1, __FUNCTION__, "font-metric");
  return fm->sub_fonts ();
}

LY_DEFINE (ly_otf_font_glyph_info, "ly:otf-font-glyph-info", 2, 0, 0,
	   (SCM font, SCM glyph),
	   "Given the font metric @var{font} of an OpenType font, return the "
	   "information about named glyph @var{glyph} (a string)")
{
  Modified_font_metric *fm
    = dynamic_cast<Modified_font_metric *> (unsmob_metrics (font));
  Open_type_font *otf = dynamic_cast<Open_type_font *> (fm->original_font ());
  SCM_ASSERT_TYPE (otf, font, SCM_ARG1, __FUNCTION__, "OTF font-metric");
  SCM_ASSERT_TYPE (scm_is_string (glyph), glyph, SCM_ARG1,
		   __FUNCTION__, "string");

  SCM sym = scm_string_to_symbol (glyph);
  return scm_hashq_ref (otf->get_char_table (), sym, SCM_EOL);
}

LY_DEFINE (ly_otf_font_table_data, "ly:otf-font-table-data", 2, 0, 0,
	   (SCM font, SCM tag),
	   "Extract a table @var{tag} from @var{font}. Return empty string for "
	   "non-existent @var{tag}.")
{
  Modified_font_metric *fm
    = dynamic_cast<Modified_font_metric *> (unsmob_metrics (font));

  Open_type_font *otf = fm ? dynamic_cast<Open_type_font *> (fm->original_font ())
    : dynamic_cast<Open_type_font *> (unsmob_metrics (font));

  SCM_ASSERT_TYPE (otf, font, SCM_ARG1, __FUNCTION__, "Open type font");
  SCM_ASSERT_TYPE (scm_is_string (tag), tag, SCM_ARG1, __FUNCTION__, "Open type font");

  char ctag [5] = "    ";

  std::string tag_string = ly_scm2string (tag);
  strncpy (ctag, tag_string.c_str (), tag_string.length ());

  std::string tab = otf->get_otf_table (std::string (ctag));

  return scm_from_locale_stringn ((char const *) tab.data (), tab.length ());
}

LY_DEFINE (ly_otf_font_p, "ly:otf-font?", 1, 0, 0,
	   (SCM font),
	   "Is @var{font} an OpenType font?")
{
  Modified_font_metric *fm
    = dynamic_cast<Modified_font_metric *> (unsmob_metrics (font));

  Open_type_font *otf = fm ? dynamic_cast<Open_type_font *> (fm->original_font ())
    : dynamic_cast<Open_type_font *> (unsmob_metrics (font));

  return scm_from_bool (otf);
}

LY_DEFINE (ly_otf_glyph_list, "ly:otf-glyph-list",
	   1, 0, 0, (SCM font),
	   "Return a list of glyphnames for @var{font}.")
{
  Modified_font_metric *fm
    = dynamic_cast<Modified_font_metric *> (unsmob_metrics (font));

  Open_type_font *otf = fm ? dynamic_cast<Open_type_font *> (fm->original_font ())
    : dynamic_cast<Open_type_font *> (unsmob_metrics (font));


  SCM_ASSERT_TYPE (otf,font, SCM_ARG1, __FUNCTION__, "OTF font");
  return otf->glyph_list ();

}
