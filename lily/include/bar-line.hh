/*
  bar.hh -- part of GNU LilyPond

  (c) 1996--2007 Han-Wen Nienhuys
*/

#ifndef BAR_HH
#define BAR_HH

#include "lily-proto.hh"
#include "grob-interface.hh"

class Bar_line
{
public:
  DECLARE_GROB_INTERFACE();

  static Stencil dashed_bar_line (Grob *me, Real h, Real thick);
  static Stencil compound_barline (Grob *, string, Real height, bool rounded);
  static Stencil simple_barline (Grob *, Real wid, Real height, bool rounded);
  DECLARE_SCHEME_CALLBACK (calc_bar_size, (SCM));
  DECLARE_SCHEME_CALLBACK (print, (SCM));
};
#endif // BAR_HH

