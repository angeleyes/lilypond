/*
  all-fonts.hh -- declare All_font_metrics

  source file of the GNU LilyPond music typesetter

  (c) 1998--2006 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#ifndef ALL_FONTS_HH
#define ALL_FONTS_HH

#include "file-path.hh"
#include "font-metric.hh"
#include "config.hh"

#if HAVE_PANGO_FT2
#include <pango/pango.h>
#include <pango/pangoft2.h>
#endif

/**
   Interface to all .afm files living in the filesystem.
*/
class All_font_metrics
{
  Scheme_hash_table *tfm_dict_;
  Scheme_hash_table *otf_dict_;
  File_path search_path_;

#if HAVE_PANGO_FT2
  PangoFT2FontMap *pango_ft2_fontmap_;
  Scheme_hash_table *pango_dict_;
  int pango_dpi_;
#endif

  All_font_metrics (All_font_metrics const &);
public:
  All_font_metrics (string search_path);
  ~All_font_metrics ();

#if HAVE_PANGO_FT2
  Pango_font *find_pango_font (PangoFontDescription *description,
			       Real magnification,
			       Real scale);
#endif

  Tex_font_metric *find_tfm (string);
  Font_metric *find_font (string name);
  Open_type_font *find_otf (string name);
  SCM font_descriptions () const;
};

extern All_font_metrics *all_fonts_global;

#endif /* ALL_FONTS_HH */

