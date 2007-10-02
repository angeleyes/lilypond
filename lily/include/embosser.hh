/*
  embosser.hh -- declare Embosser - Braille translator.

  (c) 2007 Ralph Little
*/

#ifndef EMBOSSER_HH
#define EMBOSSER_HH

#include "grob-info.hh"
#include "translator.hh"

/* Convert a music definition into a Braille representation.  */
class Embosser : public Translator
{
public:
  VIRTUAL_COPY_CONSTRUCTOR (Translator, Embosser);
  friend class Embosser_group;
  Embosser_group *get_daddy_embosser () const;

};

#endif /* EMBOSSER_HH */

