#include "context.hh"
#include "context-def.hh"


LY_DEFINE(ly_context_id,
	  "ly:context-id", 1,0,0,  (SCM context),
	  "Return the id string of @var{context}, i.e. for @code{\\context Voice "
"= one .. } it will return the string @code{one}.")
{
  Context * tr =   (unsmob_context (context));
  SCM_ASSERT_TYPE(tr, context, SCM_ARG1, __FUNCTION__, "Context");

  return scm_makfrom0str (tr->id_string_. to_str0 ());
}


LY_DEFINE(ly_context_name,
	  "ly:context-name", 1,0,0,  (SCM context),
	  "Return the name of @var{context}, i.e. for @code{\\context Voice "
"= one .. } it will return the symbol @code{Voice}.")
{
  Context * tr =   (unsmob_context (context));
  SCM_ASSERT_TYPE(tr, context, SCM_ARG1, __FUNCTION__, "Context");

  return unsmob_context_def (tr->definition_)->get_context_name (); 
}


LY_DEFINE(ly_context_pushpop_property,
	  "ly:context-pushpop-property", 3, 1, 0,
	  (SCM context, SCM grob, SCM eltprop, SCM val),
	  "Do a single @code{\\override} or @code{\\revert} operation "
	  "in @var{context}. The grob definition @code{grob} is extended with "
	  "@code{eltprop} (if @var{val} is specified) "
	  "or reverted (if  unspecified).")
{
  Context *tg =  (unsmob_context (context));

  SCM_ASSERT_TYPE(tg, context, SCM_ARG1, __FUNCTION__, "context");
  SCM_ASSERT_TYPE(gh_symbol_p (grob), grob, SCM_ARG2, __FUNCTION__, "symbol");
  SCM_ASSERT_TYPE(gh_symbol_p (eltprop), eltprop, SCM_ARG3, __FUNCTION__, "symbol");

  execute_pushpop_property (tg, grob, eltprop, val);

  return SCM_UNDEFINED;
}


LY_DEFINE(ly_context_property,
	  "ly:context-property", 2, 0, 0,
	  (SCM context, SCM name),
	  "retrieve the value of @var{name} from context @var{context}")
{
  Context *t = unsmob_context (context);
  Context * tr=    (t);
  SCM_ASSERT_TYPE(tr, context, SCM_ARG1, __FUNCTION__, "Translator group");
  SCM_ASSERT_TYPE(gh_symbol_p (name), name, SCM_ARG2, __FUNCTION__, "symbol");

  return tr->internal_get_property (name);
  
}

LY_DEFINE(ly_context_set_property,
	  "ly:context-set-property!", 3, 0, 0,
	  (SCM context, SCM name, SCM val),
	  "set value of property @var{name} in context @var{context} to @var{val}.")
{
  Context *t = unsmob_context (context);
  Context * tr=    (t);

  SCM_ASSERT_TYPE(tr, context, SCM_ARG1, __FUNCTION__, "Context");
  SCM_ASSERT_TYPE(gh_symbol_p (name), name, SCM_ARG2, __FUNCTION__, "symbol");

  tr->internal_set_property (name, val);

  return SCM_UNSPECIFIED;
}


LY_DEFINE(ly_context_property_where_defined,
	  "ly:context-property-where-defined", 2, 0, 0,
	  (SCM context, SCM name),
	  "Return the context above @var{context} where @var{name}  is defined.")
{
  Context *t = unsmob_context (context);
  Context * tr =  (t);
  SCM_ASSERT_TYPE(tr, context, SCM_ARG1, __FUNCTION__, "Context");
  SCM_ASSERT_TYPE(gh_symbol_p (name), name, SCM_ARG2, __FUNCTION__, "symbol");


  tr = tr->where_defined (name);

  if (tr)
    return tr->self_scm();

  return SCM_EOL;
}

LY_DEFINE(ly_unset_context_property,
	  "ly:unset-context-property", 2, 0, 0,
	  (SCM context, SCM name),
	  "Unset value of property @var{name} in context @var{context}.")
{
  Context *t = unsmob_context (context);
  Context * tr =  (t);
  SCM_ASSERT_TYPE(tr, context, SCM_ARG1, __FUNCTION__, "Context");
  SCM_ASSERT_TYPE(gh_symbol_p (name), name, SCM_ARG2, __FUNCTION__, "symbol");

  tr->unset_property (name);

  return SCM_UNSPECIFIED;
}



LY_DEFINE(ly_context_parent,
	  "ly:context-parent", 1, 0, 0,
	  (SCM context),
	  "Return the parent of @var{context}, #f if none.")
{
  Context *t = unsmob_context (context);
  Context * tr=    (t);

  SCM_ASSERT_TYPE(tr, context, SCM_ARG1, __FUNCTION__, "Context");

  tr =  tr->daddy_context_ ;
  if (tr)
    return tr->self_scm();
  else
    return SCM_BOOL_F;
}

/*
  Todo: should support translator IDs, and creation?
 */
LY_DEFINE(ly_translator_find,
	  "ly:translator-find", 2, 0,0,
	  (SCM context, SCM name),
	  "Find a parent of @var{context} that has name or alias @var{name}. "
	  "Return @code{#f} if not found." )
{
  Context * tr=    ( unsmob_context (context));

  SCM_ASSERT_TYPE(tr, context, SCM_ARG1, __FUNCTION__, "context");
  SCM_ASSERT_TYPE(gh_symbol_p (name), name, SCM_ARG2, __FUNCTION__, "symbol");
  
  while (tr)
    {
      if (tr->is_alias (name))
	return tr->self_scm();
      tr =  tr->daddy_context_ ;
    }
  
  return SCM_BOOL_F;
}


LY_DEFINE(ly_context_properties,
	  "ly:context-properties", 1, 0, 0,
	  (SCM context),
	  "Return all properties  of @var{context} in an alist.")
{
  Context *t = unsmob_context (context);
  Context * tr= (t);

  SCM_ASSERT_TYPE(tr, context, SCM_ARG1, __FUNCTION__, "Context");

  return tr->properties_as_alist ();
}
