/*
  rest.cc -- implement 

  source file of the GNU LilyPond music typesetter

  (c) 1997 Han-Wen Nienhuys <hanwen@stack.nl>
*/

#include "molecule.hh"
#include "paper-def.hh"
#include "lookup.hh"
#include "rest.hh"
#include "dots.hh"
#include "axis-group-element.hh"
#include "p-score.hh"

void
Rest::do_add_processing ()
{
  if (balltype_i_ == 0)
    position_i_ += 6;
  else if (balltype_i_ == 1)
    position_i_ += 4;
  Rhythmic_head::do_add_processing ();
  if (dots_l_)
      dots_l_->position_i_ = position_i_;
}

Rest::Rest ()
{
  position_i_ =0;
}

Molecule *
Rest::brew_molecule_p () const
{
  int staff_size_i_ =8;
  bool streepjes_b = (position_i_<-1) || (position_i_ > staff_size_i_+1);
  
  Atom s(paper ()->lookup_l()->rest (balltype_i_, streepjes_b));
  Molecule * m = new Molecule ( Atom (s));
  m->translate (position_i_ *  paper ()->internote_f (), Y_AXIS);
  return m;
}

IMPLEMENT_IS_TYPE_B1(Rest, Rhythmic_head);
