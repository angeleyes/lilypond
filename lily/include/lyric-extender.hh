
/*
  extender-spanner.hh -- part of GNU LilyPond

  (c) 1998--2004 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#ifndef EXTENDER_SPANNER_HH
#define EXTENDER_SPANNER_HH

#include "spanner.hh"

/*

  Extenders must be entered manually for now.

  Although it would be possible for Lily to determine where to
  put extender lines, it's quite a tricky thing to do.  Also,
  this would demand quite strict lyrics entries.

  Note: the extender is only used for one-syllable words, or
  for on a word's last syllable.  The extender should be aligned
  with the left side of the last note of the melissima, and not
  extend beond, lasting the whole duration of the melissima

 */

class Lyric_extender
{
public:
  static bool is_visible (Grob*);
  static bool has_interface (Grob*);
  DECLARE_SCHEME_CALLBACK (print, (SCM ));
};

#endif // EXTENDER_SPANNER_HH

