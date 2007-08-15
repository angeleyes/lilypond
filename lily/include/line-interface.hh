/*
  line-interface.hh -- declare

  source file of the GNU LilyPond music typesetter

  (c) 2004--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#ifndef LINE_INTERFACE_HH
#define LINE_INTERFACE_HH

#include "lily-proto.hh"

struct Line_interface
{
  static Stencil line (Grob *me, Offset from, Offset to);
  static bool has_interface (Grob *);
  static Stencil make_dashed_line (Real th, Offset from, Offset to, Real, Real);
  static Stencil make_line (Real th, Offset from, Offset to);
  static Stencil make_arrow (Offset beg, Offset end, Real thick,
			     Real length, Real width);
  static Stencil arrows (Grob *me, Offset from, Offset to,
			 bool from_arrow,
			 bool to_arrow);
};

#endif /* LINE_INTERFACE_HH */

