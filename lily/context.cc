/*   
  context.cc --  implement Context

  source file of the GNU LilyPond music typesetter

  (c) 2004 Han-Wen Nienhuys <hanwen@xs4all.nl>

*/

#include "context.hh"

#include "context-def.hh"
#include "ly-smobs.icc"
#include "main.hh"
#include "output-def.hh"
#include "scm-hash.hh"
#include "score-context.hh"
#include "translator-group.hh"
#include "warn.hh"
#include "lilypond-key.hh"

bool
Context::is_removable () const
{
  return context_list_ == SCM_EOL && ! iterator_count_ &&
	  !dynamic_cast<Score_context const*> (this);
}

void
Context::check_removal ()
{
  for (SCM p = context_list_; scm_is_pair (p); p = scm_cdr (p))
    {
      Context *trg = unsmob_context (scm_car (p));

      trg->check_removal ();
      if (trg->is_removable ())
	{
	  recurse_over_translators (trg, &Translator::finalize, UP);
	  remove_context (trg);
	}
    }
}

Context::Context (Context const&)
{
  assert (false);
}

Scheme_hash_table *
Context::properties_dict () const
{
  return Scheme_hash_table::unsmob (properties_scm_);
}

void
Context::add_context (Context *t)
{
  SCM ts = t->self_scm ();
  context_list_ = ly_append2 (context_list_,
			      scm_cons (ts, SCM_EOL));
  
  t->daddy_context_ = this;
  if (!t->init_)
    {
      t->init_ = true;

      scm_gc_unprotect_object (ts);
      Context_def *td = unsmob_context_def (t->definition_);

      /*
	this can not move before add_context (), because \override
	operations require that we are in the hierarchy.
      */
      td->apply_default_property_operations (t);

      recurse_over_translators (t, &Translator::initialize, DOWN);
    }
}

Object_key const*
Context::get_key () const
{
  return key_;
}

Context::Context (Object_key const* key)
{
  key_ = key;
  daddy_context_ = 0;
  init_ = false;
  aliases_ = SCM_EOL;
  iterator_count_  = 0;
  implementation_ = SCM_EOL;
  properties_scm_ = SCM_EOL;
  accepts_list_ = SCM_EOL;
  context_list_ = SCM_EOL;
  definition_ = SCM_EOL;
  
  smobify_self ();
  properties_scm_ = (new Scheme_hash_table)->self_scm ();
  scm_gc_unprotect_object (properties_scm_);
  scm_gc_unprotect_object (key_->self_scm ());
}

/*
  TODO:  this shares code with find_create_context().
 */
Context*
Context::create_unique_context (SCM n, SCM operations)
{
  /*
    Don't create multiple score contexts.
   */
  if (dynamic_cast<Global_context*> (this)
      && dynamic_cast<Global_context*> (this)->get_score_context ())
    return get_score_context ()->create_unique_context (n, operations);

  /*
    TODO: use accepts_list_.
   */
  Link_array<Context_def> path
    = unsmob_context_def (definition_)->path_to_acceptable_context (n, get_output_def ());

  if (path.size ())
    {
      Context * current = this;

      // start at 1.  The first one (index 0) will be us.
      for (int i= 0; i < path.size (); i++)
	{
	  SCM ops = (i == path.size () -1) ? operations : SCM_EOL;

	  current = current->create_context (path[i],
					     "\\new",
					     ops); 
	}

      return current;
    }

  /*
    Don't go up to Global_context, because global goes down to
    Score_context
   */
  Context *ret = 0;
  if (daddy_context_ && !dynamic_cast<Global_context*> (daddy_context_))
    ret = daddy_context_->create_unique_context (n, operations);
  else
    {
      warning (_f ("Cannot find or create new `%s'",
		   ly_symbol2string (n).to_str0 ()));
      ret = 0;
    }
  return ret;
}


Context *
Context::find_create_context (SCM n, String id, SCM operations)
{
  /*
    Don't create multiple score contexts.
   */
  if (dynamic_cast<Global_context*> (this)
      && dynamic_cast<Global_context*> (this)->get_score_context ())
    return get_score_context ()->find_create_context (n, id, operations);

  if (Context *existing = find_context_below (this, n, id))
    return existing;

  if (n == ly_symbol2scm ("Bottom"))
    {
      Context* tg = get_default_interpreter ();
      return tg;
    }

  /*
    TODO: use accepts_list_.
   */
  Link_array<Context_def> path
    = unsmob_context_def (definition_)->path_to_acceptable_context (n, get_output_def ());

  if (path.size ())
    {
      Context * current = this;

      // start at 1.  The first one (index 0) will be us.
      for (int i= 0; i < path.size (); i++)
	{
	  SCM ops = (i == path.size () -1) ? operations : SCM_EOL;

	  String this_id = "";
	  if (i == path.size () -1)
	    {
	      this_id = id;
	    }


	  current = current->create_context (path[i],
					     this_id,
					     ops); 
	}

      return current;
    }

  /*
    Don't go up to Global_context, because global goes down to
    Score_context
   */
  Context *ret = 0;
  if (daddy_context_ && !dynamic_cast<Global_context*> (daddy_context_))
    ret = daddy_context_->find_create_context (n, id, operations);
  else
    {
      warning (_f ("Cannot find or create `%s' called `%s'",
		   ly_symbol2string (n).to_str0 (), id));
      ret = 0;
    }
  return ret;
}


Context*
Context::create_context (Context_def * cdef,
			 String id,
			 SCM ops)
{
  String type = ly_symbol2string (cdef->get_context_name());
  Object_key const *key = get_context_key (type, id);
  Context * new_group
    = cdef->instantiate (ops, key);
	  
  new_group->id_string_ = id;
  add_context (new_group);
  apply_property_operations (new_group, ops);

  return new_group;
}



Object_key const*
Context::get_context_key (String type, String id)
{
  String now_key = type + "@" + id;

  int disambiguation_count = 0;
  if (context_counts_.find (now_key) != context_counts_.end ())
    {
      disambiguation_count = context_counts_[now_key];
    }

  context_counts_[now_key] = disambiguation_count + 1;
  
  
  return new Lilypond_context_key (get_key (),
				   now_mom(),
				   type, id,
				   disambiguation_count);
}

Object_key const*
Context::get_grob_key (String name) 
{
  int disambiguation_count = 0;
  if (grob_counts_.find (name) != grob_counts_.end ())
    {
      disambiguation_count = grob_counts_[name];
    }
  grob_counts_[name] = disambiguation_count + 1;

  Object_key * k = new Lilypond_grob_key (get_key(),
					  now_mom(),
					  name,
					  disambiguation_count);

  return k;					  
}



/*
  Default child context as a SCM string, or something else if there is
  none.
*/
SCM
Context::default_child_context_name () const
{
  return scm_is_pair (accepts_list_)
    ? scm_car (scm_last_pair (accepts_list_))
    : SCM_EOL;
}


bool
Context::is_bottom_context () const
{
  return !scm_is_symbol (default_child_context_name ());
}

Context*
Context::get_default_interpreter ()
{
  if (!is_bottom_context ())
    {
      SCM nm = default_child_context_name ();
      SCM st = find_context_def (get_output_def (), nm);

      String name = ly_symbol2string (nm);
      Context_def *t = unsmob_context_def (st);
      if (!t)
	{
	  warning (_f ("can't find or create: `%s'", name.to_str0 ()));
	  t = unsmob_context_def (this->definition_);
	}

      Context *tg = create_context (t, "", SCM_EOL);
      if (!tg->is_bottom_context ())
	return tg->get_default_interpreter ();
      else
	return tg;
    }
  return this;
}

/*
  PROPERTIES
 */
Context*
Context::where_defined (SCM sym) const
{
  if (properties_dict ()->contains (sym))
    {
      return (Context*)this;
    }

  return (daddy_context_) ? daddy_context_->where_defined (sym) : 0;
}

/*
  return SCM_EOL when not found.
*/
SCM
Context::internal_get_property (SCM sym) const
{
  SCM val = SCM_EOL;
  if (properties_dict ()->try_retrieve (sym, &val))
    return val;

  if (daddy_context_)
    return daddy_context_->internal_get_property (sym);
  
  return val;
}

bool
Context::is_alias (SCM sym) const
{
  if (sym == ly_symbol2scm ("Bottom")
      && !scm_is_pair (accepts_list_))
    return true;
  if (sym == unsmob_context_def (definition_)->get_context_name ())
    return true;
  
  return scm_c_memq (sym, aliases_) != SCM_BOOL_F;
}

void
Context::add_alias (SCM sym)
{
  aliases_ = scm_cons (sym, aliases_);
}



void
Context::internal_set_property (SCM sym, SCM val)
{
#ifndef NDEBUG
  if (internal_type_checking_global_b)
    assert (type_check_assignment (sym, val, ly_symbol2scm ("translation-type?")));
#endif
  
  properties_dict ()->set (sym, val);
}

/*
  TODO: look up to check whether we have inherited var? 
 */
void
Context::unset_property (SCM sym)
{
  properties_dict ()->remove (sym);
}

/**
   Remove a context from the hierarchy.
 */
Context *
Context::remove_context (Context*trans)
{
  assert (trans);

  context_list_ = scm_delq_x (trans->self_scm (), context_list_);
  trans->daddy_context_ = 0;
  return trans;
}

/*
  ID == "" means accept any ID.
 */
Context *
find_context_below (Context * where,
		    SCM type, String id)
{
  if (where->is_alias (type))
    {
      if (id == "" || where->id_string () == id)
	return where;
    }
  
  Context *found = 0;
  for (SCM s = where->children_contexts ();
       !found && scm_is_pair (s); s = scm_cdr (s))
    {
      Context *tr = unsmob_context (scm_car (s));

      found = find_context_below (tr, type, id);
    }

  return found; 
}

SCM
Context::properties_as_alist () const
{
  return properties_dict ()->to_alist ();
}

SCM
Context::context_name_symbol () const
{
  Context_def *td = unsmob_context_def (definition_);
  return td->get_context_name ();
}

String
Context::context_name () const
{
  return ly_symbol2string (context_name_symbol ());
}

Score_context*
Context::get_score_context () const
{
  if (Score_context *sc = dynamic_cast<Score_context*> ((Context*) this))
    return sc;
  else if (daddy_context_)
    return daddy_context_->get_score_context ();
  else
    return 0;
}

Output_def *
Context::get_output_def () const
{
  return daddy_context_ ? daddy_context_->get_output_def () : 0;
}

Context::~Context ()
{
  
}

Moment
Context::now_mom () const
{
  return daddy_context_->now_mom ();
}

int
Context::print_smob (SCM s, SCM port, scm_print_state *)
{
  Context *sc = (Context *) SCM_CELL_WORD_1 (s);
     
  scm_puts ("#<", port);
  scm_puts (classname (sc), port);
  if (Context_def *d = unsmob_context_def (sc->definition_))
    {
      scm_puts (" ", port);
      scm_display (d->get_context_name (), port);
    }

  if (Context *td=dynamic_cast<Context *> (sc))
    {
      scm_puts ("=", port);
      scm_puts (td->id_string_.to_str0 (), port);
    }
  

  scm_puts (" ", port);

  scm_display (sc->context_list_, port);
  scm_puts (" >", port);
  
  return 1;
}

SCM
Context::mark_smob (SCM sm)
{
  Context *me = (Context*) SCM_CELL_WORD_1 (sm);
  scm_gc_mark (me->key_->self_scm ());  
  scm_gc_mark (me->context_list_);
  scm_gc_mark (me->aliases_);
  scm_gc_mark (me->definition_);  
  scm_gc_mark (me->properties_scm_);  
  scm_gc_mark (me->accepts_list_);
  scm_gc_mark (me->implementation_);

  return me->properties_scm_;
}

IMPLEMENT_SMOBS (Context);
IMPLEMENT_DEFAULT_EQUAL_P (Context);
IMPLEMENT_TYPE_P (Context,"ly:context?");

bool
Context::try_music (Music* m)
{
  Translator*  t = implementation ();
  if (!t)
    return false;
  
  bool b = t->try_music (m);
  if (!b && daddy_context_)
    b = daddy_context_->try_music (m);

  return b;
}


Global_context*
Context::get_global_context () const
{
  if (dynamic_cast<Global_context *>((Context*) this))
    return dynamic_cast<Global_context *> ((Context*) this);

  if (daddy_context_)
    return daddy_context_->get_global_context ();

  programming_error ("No Global context!");
  return 0;
}

Context*
Context::get_parent_context () const
{
  return daddy_context_;
}

Translator_group*
Context::implementation () const
{
  return dynamic_cast<Translator_group*> (unsmob_translator (implementation_));
}

void
Context::clear_key_disambiguations ()
{
  grob_counts_.clear();
  context_counts_.clear();
  for (SCM s = context_list_; scm_is_pair (s); s = scm_cdr (s))
    {
      unsmob_context (scm_car (s))->clear_key_disambiguations();
    }
}
