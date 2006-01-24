/*
  paper-system.cc -- implement Paper_system

  source file of the GNU LilyPond music typesetter

  (c) 2004--2006 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include "prob.hh"
#include "main.hh"
#include "item.hh"

#include "ly-smobs.icc"

IMPLEMENT_SMOBS (Prob);
IMPLEMENT_TYPE_P (Prob, "ly:prob?");
IMPLEMENT_DEFAULT_EQUAL_P (Prob);

Prob::Prob (SCM immutable_init)
{
  self_scm_ = SCM_EOL;
  mutable_property_alist_ = SCM_EOL;
  immutable_property_alist_ = immutable_init;
  smobify_self ();
}

Prob::~Prob ()
{
}

Prob::Prob (Prob const &src)
{
  immutable_property_alist_ = src.immutable_property_alist_;
  mutable_property_alist_ = SCM_EOL;
  self_scm_ = SCM_EOL;

  /* First we smobify_self, then we copy over the stuff.  If we don't,
     stack vars that hold the copy might be optimized away, meaning
     that they won't be protected from GC. */
  smobify_self ();
  mutable_property_alist_ = src.copy_mutable_properties ();
}


SCM
Prob::copy_mutable_properties () const
{
  return ly_deep_copy (mutable_property_alist_);
}

void
Prob::derived_mark () const
{
}

SCM
Prob::mark_smob (SCM smob)
{
  Prob *system = (Prob *) SCM_CELL_WORD_1 (smob);
  scm_gc_mark (system->mutable_property_alist_);
  system->derived_mark ();
  
  return system->immutable_property_alist_;
}

int
Prob::print_smob (SCM smob, SCM port, scm_print_state*)
{
  Prob *p = (Prob *) SCM_CELL_WORD_1 (smob);
  scm_puts ("#<", port);
  scm_puts ("Prob: ", port);

  
  scm_puts (p->class_name (), port);
  scm_display (p->mutable_property_alist_, port);
  scm_display (p->immutable_property_alist_, port);
  
  scm_puts (" >\n", port);
  return 1;
}



SCM
Prob::internal_get_property (SCM sym) const
{
  /*
    TODO: type checking
   */
  SCM s = scm_sloppy_assq (sym, mutable_property_alist_);
  if (s != SCM_BOOL_F)
    return scm_cdr (s);

  s = scm_sloppy_assq (sym, immutable_property_alist_);
  return (s == SCM_BOOL_F) ? SCM_EOL : scm_cdr (s);
}

void
Prob::internal_set_property (SCM sym, SCM val) 
{
  if (do_internal_type_checking_global)
    type_check_assignment (sym, val);
  
  mutable_property_alist_ = scm_assq_set_x (mutable_property_alist_, sym, val);
}

void
Prob::type_check_assignment (SCM sym, SCM val) const
{
  (void) sym;
  (void) val;
}

SCM
Prob::get_property_alist (bool m) const
{
  return (m) ? mutable_property_alist_ : immutable_property_alist_;
}

String
Prob::name () const
{
  SCM nm = get_property ("name");
  if (scm_is_symbol (nm))
    return ly_symbol2string (nm);
  else
    return this->class_name ();
}

