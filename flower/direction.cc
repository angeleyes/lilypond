/*
  direction.cc --  implement Direction

  source file of the GNU LilyPond music typesetter

  (c) 1998--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "direction.hh"

#include "string.hh"

String
direction_string (Direction d, Axis a)
{
  String s ("center");
  if (a == Y_AXIS)
    {
      s = (d == UP ? "up" : "down");
    }
  else if (a == X_AXIS)
    {
      s = (d == LEFT ? "left" : "right");
    }
  return s;
}
