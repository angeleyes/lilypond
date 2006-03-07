/*
  multi-measure-rest.hh -- declare Multi_measure_rest

  source file of the GNU LilyPond music typesetter

  (c) 1998--2006 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#ifndef MULTI_MEASURE_REST_HH
#define MULTI_MEASURE_REST_HH

#include "lily-guile.hh"
#include "rod.hh"

class Multi_measure_rest
{
public:
  static bool has_interface (Grob *);
  DECLARE_SCHEME_CALLBACK (print, (SCM));
  DECLARE_SCHEME_CALLBACK (percent, (SCM));
  static void add_column (Grob *, Item *);
  DECLARE_SCHEME_CALLBACK (set_spacing_rods, (SCM));
  DECLARE_SCHEME_CALLBACK (set_text_rods, (SCM));
  DECLARE_SCHEME_CALLBACK (centered_stencil, (SCM));


  static void calculate_spacing_rods (Grob *me, Real length);
  static Stencil big_rest (Grob *, Real);
  static Stencil symbol_stencil (Grob *, Real);
  static Stencil church_rest (Grob *, Font_metric *, int, Real);
};

#endif /* MULTI_MEASURE_REST_HH */

