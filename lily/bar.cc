/*
  bar.cc -- implement Bar

  source file of the GNU LilyPond music typesetter

  (c) 1997 Han-Wen Nienhuys <hanwen@stack.nl>
*/

#include "bar.hh"
#include "string.hh"
#include "molecule.hh"
#include "paper-def.hh"
#include "lookup.hh"
#include "debug.hh"

Bar::Bar()
{
  breakable_b_ = true;
  type_str_ = "|";
  spanned_i_ = 0;
}



IMPLEMENT_IS_TYPE_B1(Bar,Item);

void
Bar::do_print() const
{
  DOUT << type_str_;
}

Molecule*
Bar::brew_molecule_p() const
{    
  Paper_def *p = paper();
  Atom s = p->lookup_l()->bar (type_str_, 
			       p->get_var ("bar_size"));
   s.translate_axis (-s.extent()[Y_AXIS].center (), Y_AXIS);
  
  Molecule*output = new Molecule (Atom (s));
  return output;
}

/**
  Prescriptions for splitting bars.
  TODO: parametrise this (input-settable)
 */
static char const *bar_breaks[][3] ={
  {":|", ":|:", "|:"},
  {"|", "|", ""},
  {"", "|:", "|:"},
  {"||.", "||.", ""},
  {":|", ":|", ""},
  {0,0,0}
};

void
Bar::do_pre_processing()
{
  for (int i=0; bar_breaks[i][0]; i++) 
    {
	if (bar_breaks[i][1] == type_str_)
	    type_str_ = bar_breaks[i][break_status_i()+1];
    }
  
  /*
    span_score_bar needs dims, so don't do
  
    transparent_b_ = empty_b_ = (!type_str_);
    
    */
}
  
