/*
  swallow-reg.cc -- implement Swallow_engraver

  source file of the GNU LilyPond music typesetter

  (c) 1997--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "engraver.hh"

/**
 */
class Swallow_engraver : public Engraver
{
public:
  TRANSLATOR_DECLARATIONS(Swallow_engraver);
protected:
  bool try_music (Music*) ;
};



bool
Swallow_engraver::try_music (Music*) 
{
  return true;
}

Swallow_engraver::Swallow_engraver ()
{
}

ENTER_DESCRIPTION(Swallow_engraver,
/* descr */       "This engraver swallows everything given to it silently. The purpose of "
		  "this is to prevent spurious \"event junked\" warnings.",
/* creats*/       "",
/* accepts */     "general-music",
/* acks  */       "",
/* reads */       "",
/* write */       "");
