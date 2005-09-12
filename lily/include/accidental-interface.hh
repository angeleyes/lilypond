/*
  accidental-interface.hh -- declare  Accidental_interface

  source file of the GNU LilyPond music typesetter

  (c) 2002--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#ifndef ACCIDENTAL_INTERFACE_HH
#define ACCIDENTAL_INTERFACE_HH

#include "box.hh"
#include "lily-guile.hh"
#include "lily-proto.hh"

class Accidental_interface
{
public:
  DECLARE_SCHEME_CALLBACK (print, (SCM));
  DECLARE_SCHEME_CALLBACK (after_line_breaking, (SCM));
  
  static bool has_interface (Grob *);
  static String get_fontcharname (String style, int alteration);
  static Array<Box> Accidental_interface::accurate_boxes (Grob *me,
							  Grob **common);
};

#endif
