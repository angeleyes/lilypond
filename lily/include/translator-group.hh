/*
  translator-group.hh -- declare Translator_group

  source file of the GNU LilyPond music typesetter

  (c) 1997--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/


#ifndef TRANSLATOR_GROUP_HH
#define TRANSLATOR_GROUP_HH

#include "string.hh"
#include "lily-proto.hh"
#include "virtual-methods.hh"
#include "translator.hh"
#include "parray.hh"
#include "smobs.hh"

typedef void (Translator::*Translator_method) (void);

class Translator_group : public virtual Translator {
protected:
  
public:
  VIRTUAL_COPY_CONS (Translator);

public:
  virtual Translator_group* get_daddy_translator ()const;
  virtual SCM get_simple_trans_list ();
  virtual bool try_music (Music* req);       
  virtual void initialize ();
};


SCM names_to_translators (SCM namelist, Context*tg);
void recurse_down_translators (Context * c, Translator_method ptr, Direction);
void translator_each (SCM list, Translator_method method);


#endif // TRANSLATOR_GROUP_HH
