/*
  note-head-scheme.cc -- implement Note_head bindings.

  source file of the GNU LilyPond music typesetter

  (c) 2006 Han-Wen Nienhuys <hanwen@lilypond.org>

*/

#include "note-head.hh"
#include "font-metric.hh"


LY_DEFINE(ly_note_head__stem_attachment, "ly:note-head::stem-attachment",
	  2, 0, 0, (SCM font_metric, SCM glyph_name),
	  "Get attachment in @var{font-metric} for attaching a stem to notehead "
	  "@var{glyph-name}.")
{
  Font_metric *fm = unsmob_metrics (font_metric);
  SCM_ASSERT_TYPE(fm, font_metric, SCM_ARG1, __FUNCTION__, "font metric");
  SCM_ASSERT_TYPE(scm_is_string (glyph_name), glyph_name, SCM_ARG2, __FUNCTION__, "string");
  
  return ly_offset2scm (Note_head::get_stem_attachment (fm, ly_scm2string (glyph_name)));
}

