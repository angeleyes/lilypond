/*
  translator-group.hh -- declare Translator_group

  source file of the GNU LilyPond music typesetter

  (c) 1997--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#ifndef TRANSLATOR_GROUP_HH
#define TRANSLATOR_GROUP_HH

#include "listener.hh"
#include "translator.hh"
#include "std-vector.hh"

typedef void (Translator:: *Translator_method) (void);
typedef void (Translator_group:: *Translator_group_method) (void);
typedef void (*Translator_group_void_method) (Translator_group *);

struct Translator_method_binding
{
  Translator *translator_;
  Translator_void_method_ptr method_;

  Translator_method_binding ()
  {
  }
  Translator_method_binding (Translator *tr, Translator_void_method_ptr ptr)
  {
    translator_ = tr;
    method_ = ptr;
  }
  void invoke ()
  {
    if (method_)
      (*method_) (translator_);
  }
};

class Translator_group
{
private:
  void precompute_method_bindings ();
  vector<Translator_method_binding>
  precomputed_method_bindings_[TRANSLATOR_METHOD_PRECOMPUTE_COUNT];

  Translator_group_void_method
  precomputed_self_method_bindings_[TRANSLATOR_METHOD_PRECOMPUTE_COUNT];

  SCM protected_events_;

  DECLARE_LISTENER (create_child_translator);

public:
  VIRTUAL_COPY_CONSTRUCTOR (Translator_group, Translator_group);
  DECLARE_SMOBS (Translator_group);

public:
  virtual void connect_to_context (Context *c);
  virtual void disconnect_from_context ();
  virtual Translator_group *get_daddy_translator ()const;
  virtual SCM get_simple_trans_list ();
  virtual void initialize ();
  virtual void finalize ();

  void protect_event (SCM ev);

  void stop_translation_timestep ();
  void start_translation_timestep ();

  virtual void fetch_precomputable_methods (Translator_group_void_method[]);

  Translator_group ();

  void precomputed_translator_foreach (Translator_precompute_index);
  void call_precomputed_self_method (Translator_precompute_index);

  Context *context () const { return context_; }
protected:
  SCM simple_trans_list_;
  Context *context_;

  friend class Context_def;
  virtual void derived_mark () const;
};

SCM names_to_translators (SCM namelist, Context *tg);
void recurse_over_translators (Context *c, Translator_method ptr,
			       Translator_group_method ptr2, Direction);
void precomputed_recurse_over_translators (Context *c, Translator_precompute_index idx, Direction dir);
Translator_group *get_translator_group (SCM sym);

#define foobar
#define ADD_TRANSLATOR_GROUP(classname, desc, grobs, read, write) foobar

DECLARE_UNSMOB (Translator_group, translator_group);

#endif // TRANSLATOR_GROUP_HH
