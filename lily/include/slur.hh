/*
  slur.hh -- declare Slur
  
  source file of the GNU LilyPond music typesetter
  
  (c) 2004 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#ifndef SLUR_HH
#define SLUR_HH

#include "lily-proto.hh"
#include "lily-guile.hh"

class Slur
{
public:
  static void add_column (Grob *me, Grob *col);
  static void add_extra_encompass (Grob *me, Grob *col);
  DECLARE_SCHEME_CALLBACK (print, (SCM));
  DECLARE_SCHEME_CALLBACK (after_line_breaking, (SCM));
  DECLARE_SCHEME_CALLBACK (height, (SCM,SCM));
  DECLARE_SCHEME_CALLBACK (outside_slur_callback, (SCM,SCM));
  static bool has_interface (Grob *);
  static Bezier get_curve (Grob*me);
};

#endif /* SLUR_HH */
