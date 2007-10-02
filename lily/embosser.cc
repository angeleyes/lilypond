/*
  embosser.cc -- implement Embosser - translator for Braille

  source file of the GNU LilyPond music typesetter

  (c) 2007 Ralph Little
*/

#include "context.hh"
#include "embosser-group.hh"
#include "warn.hh"

Embosser_group *
Embosser::get_daddy_embosser () const
{
  return dynamic_cast<Embosser_group *> (get_daddy_translator ());
}



