/*
  volta-spanner.hh -- part of GNU LilyPond

  (c) 1997--2005 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#ifndef VOLTA_SPANNER_HH
#define VOLTA_SPANNER_HH

#include "spanner.hh"

class Volta_bracket_interface
{
public:
  static bool has_interface (Grob *);
  DECLARE_SCHEME_CALLBACK (print, (SCM));
  DECLARE_SCHEME_CALLBACK (after_line_breaking, (SCM));
  static void add_column (Grob *, Grob *col);
  static void add_bar (Grob *me, Item *bar);
};

#endif // VOLTA_SPANNER_HH

