/*
  lilypond-version.hh -- declare LilyPond_version

  source file of the GNU LilyPond music typesetter

  (c) 1998--2005 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#ifndef LILYPOND_VERSION_HH
#define LILYPOND_VERSION_HH

#include "string.hh"

struct Lilypond_version
{
  Lilypond_version (int major, int minor, int patch);
  Lilypond_version (String str);

  String to_string () const;
  operator int () const;

  int major_;
  int minor_;
  int patch_;
  String extra_patch_string_;
};

extern Lilypond_version oldest_version;

#endif // LILYPOND_VERSION_HH
