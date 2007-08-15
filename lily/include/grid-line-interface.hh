/*
  grid-line-interface.hh -- declare Grid_line_interface

  source file of the GNU LilyPond music typesetter

  (c) 2005--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#ifndef GRID_LINE_INTERFACE_HH
#define GRID_LINE_INTERFACE_HH

#include "lily-proto.hh"
#include "lily-guile.hh"

class Grid_line_interface
{
public:
  static void add_grid_point (Grob *me, Grob *b);
  DECLARE_SCHEME_CALLBACK (print, (SCM));
  DECLARE_SCHEME_CALLBACK (width, (SCM));
  static bool has_interface (Grob *);
};

class Grid_point_interface
{
public:
  static bool has_interface (Grob *);
};

#endif /* GRID_LINE_INTERFACE_HH */

