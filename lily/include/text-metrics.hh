/*
  text-metrics.hh -- declare text metric lookup functions

  source file of the GNU LilyPond music typesetter

  (c) 2004--2005 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#ifndef TEXT_METRICS_HH
#define TEXT_METRICS_HH

#include "lily-guile.hh"
#include "box.hh"

void try_load_text_metrics (String);
SCM ly_load_text_dimensions (SCM);
Box lookup_tex_text_dimension (Font_metric *font,
			       SCM text);

#endif /* TEXT_METRICS_HH */

