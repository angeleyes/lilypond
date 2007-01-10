/*
  Implement storage and manipulation of grob properties.
*/

#include <cstring>

#include "main.hh"
#include "input.hh"
#include "pointer-group-interface.hh"
#include "misc.hh"
#include "paper-score.hh"
#include "output-def.hh"
#include "spanner.hh"
#include "international.hh"
#include "item.hh"
#include "misc.hh"
#include "item.hh"
#include "program-option.hh"
#include "profile.hh"
#include "simple-closure.hh"
#include "warn.hh"

#ifndef NDEBUG
static SCM modification_callback = SCM_EOL;

LY_DEFINE (ly_set_grob_modification_callback, "ly:set-grob-modification-callback",
	   1, 0, 0, (SCM cb),
	   "Specify a procedure that will be called every time lilypond modifies "
	   "a grob property. The callback will receive as arguments "
	   "the grob that is being modified, the name of the C++ file in which "
	   "the modification was requested, the line number in the C++ file in "
	   "which the modification was requested, the property to be changed and "
	   "the new value for the property.")
{

  SCM_ASSERT_TYPE(ly_is_procedure (cb), cb, SCM_ARG1, __FUNCTION__,
		  "procedure");

  modification_callback = cb;
  return SCM_UNSPECIFIED;
}
#endif

SCM
Grob::get_property_alist_chain (SCM def) const
{
  return scm_list_n (mutable_property_alist_,
		     immutable_property_alist_,
		     def,
		     SCM_UNDEFINED);
}


extern void check_interfaces_for_property (Grob const *me, SCM sym);

#ifndef NDEBUG
void
Grob::internal_set_property (SCM sym, SCM v, char const *file, int line, char const *fun)
{
  SCM grob_p = ly_lily_module_constant ("ly:grob?");
  SCM grob_list_p = ly_lily_module_constant ("grob-list?");
  SCM type = scm_object_property (sym, ly_symbol2scm ("backend-type?"));

  if (type == grob_p
      || type == grob_list_p
      || (unsmob_grob (v) && ly_symbol2scm ("cause") != sym))
    {
      scm_display (scm_list_2 (sym, type), scm_current_output_port ());
      assert (0);
    }
  
  internal_set_value_on_alist (&mutable_property_alist_,
			       sym, v);


  if (ly_is_procedure (modification_callback))
    scm_apply_0 (modification_callback,
		 scm_list_n (self_scm (),
			     scm_from_locale_string (file),
			     scm_from_int (line),
			     scm_from_locale_string (fun),
			     sym, v, SCM_UNDEFINED));
}
#else
void
Grob::internal_set_property (SCM sym, SCM v)
{
  internal_set_value_on_alist (&mutable_property_alist_,
			       sym, v);

}
#endif

void
Grob::internal_set_value_on_alist (SCM *alist, SCM sym, SCM v)
{
  /* Perhaps we simply do the assq_set, but what the heck. */
  if (!is_live ())
    return;

  if (do_internal_type_checking_global)
    {
      if (!ly_is_procedure (v)
	  && !is_simple_closure (v)
	  && v != ly_symbol2scm ("calculation-in-progress"))
	type_check_assignment (sym, v, ly_symbol2scm ("backend-type?"));

      check_interfaces_for_property (this, sym);
    }

  *alist = scm_assq_set_x (*alist, sym, v);
}

SCM
Grob::internal_get_property_data (SCM sym) const
{
#ifndef NDEBUG
  if (profile_property_accesses)
    note_property_access (&grob_property_lookup_table, sym);
#endif
  
  SCM handle = scm_sloppy_assq (sym, mutable_property_alist_);
  if (handle != SCM_BOOL_F)
    return scm_cdr (handle);

  handle = scm_sloppy_assq (sym, immutable_property_alist_);

  if (do_internal_type_checking_global && scm_is_pair (handle))
    {
      SCM val = scm_cdr (handle);
      if (!ly_is_procedure (val) && !is_simple_closure (val))
	type_check_assignment (sym, val, ly_symbol2scm ("backend-type?"));

      check_interfaces_for_property (this, sym);
    }
  
  return (handle == SCM_BOOL_F) ? SCM_EOL : scm_cdr (handle);
}

SCM
Grob::internal_get_property (SCM sym) const
{
  SCM val = get_property_data (sym);
  if (ly_is_procedure (val)
      || is_simple_closure (val))
    {
      Grob *me = ((Grob*)this);
      val = me->try_callback_on_alist (&me->mutable_property_alist_, sym, val);
    }
  
  return val;
}

#ifndef NDEBUG
#include "protected-scm.hh"

Protected_scm grob_property_callback_stack = SCM_EOL;
bool debug_property_callbacks = 0;
#endif

SCM
Grob::try_callback_on_alist (SCM *alist, SCM sym, SCM proc)
{      
  SCM marker = ly_symbol2scm ("calculation-in-progress");
  /*
    need to put a value in SYM to ensure that we don't get a
    cyclic call chain.
  */
  *alist = scm_assq_set_x (*alist, sym, marker);

#ifndef NDEBUG
  if (debug_property_callbacks)
    grob_property_callback_stack = scm_acons (sym, proc, grob_property_callback_stack);
#endif

  SCM value = SCM_EOL;
  if (ly_is_procedure (proc))
    value = scm_call_1 (proc, self_scm ());
  else if (is_simple_closure (proc))
    {
      value = evaluate_with_simple_closure (self_scm (),
					    simple_closure_expression (proc),
					    false, 0, 0);
    }
  
#ifndef NDEBUG
  if (debug_property_callbacks)
    grob_property_callback_stack = scm_cdr (grob_property_callback_stack);
#endif
	  
  /*
    If the function returns SCM_UNSPECIFIED, we assume the
    property has been set with an explicit set_property()
    call.
  */
  if (value == SCM_UNSPECIFIED)
    {
      value = internal_get_property (sym);
      assert (value == SCM_EOL || value == marker);
      if (value == marker)
	*alist = scm_assq_remove_x (*alist, marker);
    }
  else
    internal_set_value_on_alist (alist, sym, value);
  
  return value;
}

void
Grob::internal_set_object (SCM s, SCM v)
{
  /* Perhaps we simply do the assq_set, but what the heck. */
  if (!is_live ())
    return;

  object_alist_ = scm_assq_set_x (object_alist_, s, v);
}

void
Grob::internal_del_property (SCM sym)
{
  mutable_property_alist_ = scm_assq_remove_x (mutable_property_alist_, sym);
}

SCM
Grob::internal_get_object (SCM sym) const
{
  if (profile_property_accesses)
    note_property_access (&grob_property_lookup_table, sym);

  SCM s = scm_sloppy_assq (sym, object_alist_);
  
  if (s != SCM_BOOL_F)
    {
      SCM val = scm_cdr (s);
      if (ly_is_procedure (val)
	  || is_simple_closure (val))
	{
	  Grob *me = ((Grob*)this);
	  val = me->try_callback_on_alist (&me->object_alist_, sym, val);
	}
      
      return val;
    }

  return SCM_EOL;
}

bool
Grob::is_live () const
{
  return scm_is_pair (immutable_property_alist_);
}

bool
Grob::internal_has_interface (SCM k)
{
  return scm_c_memq (k, interfaces_) != SCM_BOOL_F;
}

SCM
call_pure_function (SCM unpure, SCM args, int start, int end)
{
  SCM scm_call_pure_function = ly_lily_module_constant ("call-pure-function");

  return scm_apply_0 (scm_call_pure_function,
		      scm_list_4 (unpure, args, scm_from_int (start), scm_from_int (end)));
}
