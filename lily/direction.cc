
/*   
  direction.cc --  implement 
  
  source file of the GNU LilyPond music typesetter
  
  (c) 1998 Han-Wen Nienhuys <hanwen@cs.ruu.nl>
  
 */

#include "direction.hh"

String direction_str (Direction d, Axis a)
{
  if (!d)
    return "center";
  if (a == Y_AXIS)
    {
       return d == UP ? "up" : "down";
    }
  else if (a == X_AXIS)
    {
      return d == LEFT ? "left" : "right" ;
    }
}
