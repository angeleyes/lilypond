/*
  arpegio.hh -- declare Arpeggio

  source file of the GNU LilyPond music typesetter

  (c) 2000--2006 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#ifndef ARPEGGIO_HH
#define ARPEGGIO_HH

#include "lily-proto.hh"
#include "grob-interface.hh"


class Arpeggio
{
public:
  DECLARE_SCHEME_CALLBACK (print, (SCM));
  DECLARE_SCHEME_CALLBACK (brew_chord_bracket, (SCM));
  DECLARE_SCHEME_CALLBACK (width, (SCM));
  DECLARE_GROB_INTERFACE();
};

#endif /* ARPEGGIO_HH */
