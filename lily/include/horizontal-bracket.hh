/*
  horizontal-bracket.hh -- declare Horizontal_bracket

  source file of the GNU LilyPond music typesetter

  (c) 2005 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#ifndef HORIZONTAL_BRACKET_HH
#define HORIZONTAL_BRACKET_HH

#include "lily-guile.hh"
#include "lily-proto.hh"

struct Horizontal_bracket
{
  DECLARE_SCHEME_CALLBACK (print, (SCM));
  static Stencil make_bracket (Grob *, Real, Axis, Direction);
  static Stencil make_enclosing_bracket (Grob *me, Grob *refpoint,
					 Link_array<Grob> grobs,
					 Axis a, Direction dir);
  static bool has_interface (Grob *);
};

#endif /* HORIZONTAL_BRACKET_HH */
