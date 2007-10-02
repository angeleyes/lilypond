/*
  embosser-group.hh -- declare Embosser_group

  (c) 2007 Ralph Little
*/

#ifndef EMBOSSER_GROUP_HH
#define EMBOSSER_GROUP_HH

#include "embosser.hh"
#include "translator-group.hh"

class Embosser_group : public virtual Translator_group
{
public:
  VIRTUAL_COPY_CONSTRUCTOR (Translator_group, Embosser_group);
  void do_announces ();

};

#endif /* EMBOSSER_GROUP_HH */
