/*   
  rod.cc --  implement Rod, Column_rod
  
  source file of the GNU LilyPond music typesetter
  
  (c) 1998--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  
 */

#include "rod.hh"

#include "paper-column.hh"
#include "warn.hh"
#include "dimensions.hh"
#include "spaceable-grob.hh"

Rod::Rod ()
{
  distance_ = 0.0;
  item_l_drul_[LEFT] = item_l_drul_[RIGHT] = 0;
}



void
Rod::columnize ()
{
  if (!item_l_drul_[LEFT]
      || !item_l_drul_[RIGHT])
    return ;
  
  Direction d = LEFT;
  do {
    Paper_column * pc = item_l_drul_[d]->get_column ();
    distance_ += - d * item_l_drul_[d]->relative_coordinate (pc, X_AXIS);
    item_l_drul_[d] = pc;
  } while ((flip (&d))!= LEFT);

}

void
Rod::add_to_cols ()
{
  columnize ();
  if (item_l_drul_[LEFT] != item_l_drul_[RIGHT]
      && item_l_drul_[LEFT] && item_l_drul_[RIGHT])
    Spaceable_grob::add_rod (item_l_drul_[LEFT],
			     item_l_drul_[RIGHT],
			     distance_);
}

