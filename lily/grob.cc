/*
  grob.cc -- implement Grob

  source file of the GNU LilyPond music typesetter

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "grob.hh"

#include <cstring>

#include "main.hh"
#include "input-smob.hh"
#include "warn.hh"
#include "pointer-group-interface.hh"
#include "misc.hh"
#include "paper-score.hh"
#include "stencil.hh"
#include "warn.hh"
#include "system.hh"
#include "item.hh"
#include "stencil.hh"
#include "misc.hh"
#include "music.hh"
#include "item.hh"
#include "paper-score.hh"
#include "ly-smobs.icc"
#include "output-def.hh"




MAKE_SCHEME_CALLBACK(Grob, y_parent_positioning, 1);
SCM
Grob::y_parent_positioning (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  Grob *par = me->get_parent (Y_AXIS);
  if (par)
    (void) par->get_property ("positioning-done");

  return scm_from_double (0.0);
}


MAKE_SCHEME_CALLBACK(Grob, x_parent_positioning, 1);
SCM
Grob::x_parent_positioning (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  
  Grob *par = me->get_parent (X_AXIS);
  if (par)
    (void) par->get_property ("positioning-done");

  return scm_from_double (0.0);
}

Grob *
Grob::clone (int count) const
{
  return new Grob (*this, count);
}

/* TODO:

- remove dynamic_cast<Spanner, Item> and put this code into respective
subclass.  */

#define HASH_SIZE 3
#define INFINITY_MSG "Infinity or NaN encountered"

Grob::Grob (SCM basicprops,
	    Object_key const *key)
{
  key_ = key;
  /* FIXME: default should be no callback.  */
  self_scm_ = SCM_EOL;
  layout_ = 0;
  original_ = 0;
  interfaces_ = SCM_EOL;
  immutable_property_alist_ = basicprops;
  mutable_property_alist_ = SCM_EOL;
  object_alist_ = SCM_EOL;
  
  /* We do smobify_self () as the first step.  Since the object lives
     on the heap, none of its SCM variables are protected from
     GC. After smobify_self (), they are.  */
  smobify_self ();

  /*
    We always get a new key object for a new grob.
  */
  if (key_)
    ((Object_key *)key_)->unprotect ();

  SCM meta = get_property ("meta");
  if (scm_is_pair (meta))
    interfaces_ = scm_cdr (scm_assq (ly_symbol2scm ("interfaces"), meta));
  
  if (get_property_data (ly_symbol2scm ("X-extent")) == SCM_EOL)
    set_property ("X-extent", Grob::stencil_width_proc);
  if (get_property_data (ly_symbol2scm ("Y-extent")) == SCM_EOL)
    set_property ("Y-extent", Grob::stencil_height_proc);
}

Grob::Grob (Grob const &s, int copy_index)
  : dim_cache_ (s.dim_cache_)
{
  key_ = (use_object_keys) ? new Copied_key (s.key_, copy_index) : 0;
  original_ = (Grob *) & s;
  self_scm_ = SCM_EOL;

  immutable_property_alist_ = s.immutable_property_alist_;
  mutable_property_alist_ = ly_deep_copy (s.mutable_property_alist_);
  interfaces_ = s.interfaces_;
  object_alist_ = SCM_EOL;

  layout_ = 0;

  smobify_self ();
  if (key_)
    ((Object_key *)key_)->unprotect ();
}

Grob::~Grob ()
{
}


static SCM
grob_stencil_extent (Grob *me, Axis a)
{
  Stencil *m = me->get_stencil ();
  Interval e;
  if (m)
    e = m->extent (a);
  return ly_interval2scm (e);
}


MAKE_SCHEME_CALLBACK (Grob, stencil_height, 1);
SCM
Grob::stencil_height (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  return grob_stencil_extent (me, Y_AXIS);
}

MAKE_SCHEME_CALLBACK (Grob, stencil_width, 1);
SCM
Grob::stencil_width (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  return grob_stencil_extent (me, X_AXIS);
}


Interval
robust_relative_extent (Grob *me, Grob *refpoint, Axis a)
{
  Interval ext = me->extent (refpoint, a);
  if (ext.is_empty ())
    ext.add_point (me->relative_coordinate (refpoint, a));

  return ext;
}


Stencil *
Grob::get_stencil () const
{
  if (!is_live ())
    return 0;

  SCM stil = get_property ("stencil");
  return unsmob_stencil (stil);
}

Stencil
Grob::get_print_stencil () const
{
  SCM stil = get_property ("stencil");

  Stencil retval;
  if (Stencil *m = unsmob_stencil (stil))
    {
      retval = *m;
      if (to_boolean (get_property ("transparent")))
	retval = Stencil (m->extent_box (), SCM_EOL);
      else
	{
	  SCM expr = m->expr ();
	  if (point_and_click_global)
	    expr = scm_list_3 (ly_symbol2scm ("grob-cause"),
			       self_scm (), expr);

	  retval = Stencil (m->extent_box (), expr);
	}

      /* color support... see interpret_stencil_expression () for more... */
      SCM color = get_property ("color");
      if (color != SCM_EOL)
	{
	  m = unsmob_stencil (stil);
	  SCM expr = scm_list_3 (ly_symbol2scm ("color"),
				 color,
				 m->expr ());

	  retval = Stencil (m->extent_box (), expr);
	}

    }

  return retval;
}

/*
  VIRTUAL STUBS
*/
void
Grob::do_break_processing ()
{
}

System *
Grob::get_system () const
{
  return 0;
}


void
Grob::handle_broken_dependencies ()
{
  Spanner *sp = dynamic_cast<Spanner *> (this);
  if (original () && sp)
    return;

  if (sp)
    /* THIS, SP is the original spanner.  We use a special function
       because some Spanners have enormously long lists in their
       properties, and a special function fixes FOO  */
    {
      for (SCM s = object_alist_; scm_is_pair (s); s = scm_cdr (s))
	sp->substitute_one_mutable_property (scm_caar (s), scm_cdar (s));
    }
  System *system = get_system ();

  if (is_live ()
      && system
      && common_refpoint (system, X_AXIS)
      && common_refpoint (system, Y_AXIS))
    substitute_object_links (system->self_scm (), object_alist_);
  else if (dynamic_cast<System *> (this))
    substitute_object_links (SCM_UNDEFINED, object_alist_);
  else
    /* THIS element is `invalid'; it has been removed from all
       dependencies, so let's junk the element itself.

       Do not do this for System, since that would remove references
       to the originals of score-grobs, which get then GC'd (a bad
       thing).  */
    suicide ();
}

/* Note that we still want references to this element to be
   rearranged, and not silently thrown away, so we keep pointers like
   {broken_into_{drul, array}, original}
*/
void
Grob::suicide ()
{
  if (!is_live ())
    return;

  mutable_property_alist_ = SCM_EOL;
  object_alist_ = SCM_EOL;
  immutable_property_alist_ = SCM_EOL;
  interfaces_ = SCM_EOL;
}

void
Grob::handle_prebroken_dependencies ()
{
  /* Don't do this in the derived method, since we want to keep access to
     object_alist_ centralized.  */
  if (original ())
    {
      Item *it = dynamic_cast<Item *> (this);
      substitute_object_links (scm_from_int (it->break_status_dir ()),
			       original ()->object_alist_);
    }
}

Grob *
Grob::find_broken_piece (System *) const
{
  return 0;
}

/* Translate in one direction.  */
void
Grob::translate_axis (Real y, Axis a)
{
  if (isinf (y) || isnan (y))
    {
      programming_error (_ (INFINITY_MSG));
      return ;
    }
  
  if (!dim_cache_[a].offset_)
    dim_cache_[a].offset_ = new Real (y);
  else
    *dim_cache_[a].offset_ += y;  
}

/* Find the offset relative to D.  If D equals THIS, then it is 0.
   Otherwise, it recursively defd as

   OFFSET_ + PARENT_L_->relative_coordinate (D) */
Real
Grob::relative_coordinate (Grob const *refp, Axis a) const
{
  if (refp == this)
    return 0.0;

  /* We catch PARENT_L_ == nil case with this, but we crash if we did
     not ask for the absolute coordinate (ie. REFP == nil.)  */
  Real off = get_offset (a);
  if (refp == dim_cache_[a].parent_)
    return off;
  
  off += dim_cache_[a].parent_->relative_coordinate (refp, a);

  return off;
}

/* Invoke callbacks to get offset relative to parent.  */
Real
Grob::get_offset (Axis a) const
{
  if (dim_cache_[a].offset_)
    return *dim_cache_[a].offset_;

  Grob *me = (Grob *) this;

  SCM sym = axis_offset_symbol (a);
  me->dim_cache_[a].offset_ = new Real (0.0);

  /*
    UGH: can't fold next 2 statements together. Apparently GCC thinks
    dim_cache_[a].offset_ is unaliased.
  */
  Real off = robust_scm2double (internal_get_property (sym), 0.0);
  *me->dim_cache_[a].offset_ += off;

  me->del_property (sym);
  return *me->dim_cache_[a].offset_;
}

void
Grob::flush_extent_cache (Axis axis)
{
  if (dim_cache_[axis].extent_)
    {
      /*
	Ugh, this is not accurate; will flush property, causing
	callback to be called if.
       */
      del_property ((axis == X_AXIS) ? ly_symbol2scm ("X-extent") : ly_symbol2scm ("Y-extent"));
      delete dim_cache_[axis].extent_;
      dim_cache_[axis].extent_ = 0;
      if (get_parent (axis))
	get_parent (axis)->flush_extent_cache (axis);
    }
}


Interval
Grob::extent (Grob *refp, Axis a) const
{
  Real offset = relative_coordinate (refp, a);
  Interval real_ext;
  if (dim_cache_[a].extent_)
    {
      real_ext = *dim_cache_[a].extent_;
    }
  else
    {
      SCM min_ext_sym =
	(a == X_AXIS)
	? ly_symbol2scm ("minimum-X-extent")
	: ly_symbol2scm ("minimum-Y-extent");

      SCM ext_sym =
	(a == X_AXIS)
	? ly_symbol2scm ("X-extent")
	: ly_symbol2scm ("Y-extent");
  
      SCM min_ext = internal_get_property (min_ext_sym);
      SCM ext = internal_get_property (ext_sym);

      if (is_number_pair (min_ext))
	real_ext.unite (ly_scm2interval (min_ext));
      if (is_number_pair (ext))
	real_ext.unite (ly_scm2interval (ext));

      ((Grob*)this)->del_property (ext_sym);
      ((Grob*)this)->dim_cache_[a].extent_ = new Interval (real_ext);  
    }
  
  real_ext.translate (offset);
  
  return real_ext;
}

/* Find the group-element which has both #this# and #s#  */
Grob *
Grob::common_refpoint (Grob const *s, Axis a) const
{
  /* I don't like the quadratic aspect of this code, but I see no
     other way.  The largest chain of parents might be 10 high or so,
     so it shouldn't be a real issue.  */
  for (Grob const *c = this; c; c = c->dim_cache_[a].parent_)
    for (Grob const *d = s; d; d = d->dim_cache_[a].parent_)
      if (d == c)
	return (Grob *) d;

  return 0;
}

Grob *
common_refpoint_of_list (SCM elist, Grob *common, Axis a)
{
  for (; scm_is_pair (elist); elist = scm_cdr (elist))
    if (Grob *s = unsmob_grob (scm_car (elist)))
      {
	if (common)
	  common = common->common_refpoint (s, a);
	else
	  common = s;
      }

  return common;
}

Grob *
common_refpoint_of_array (Link_array<Grob> const &arr, Grob *common, Axis a)
{
  for (int i = arr.size (); i--;)
    if (Grob *s = arr[i])
      {
	if (common)
	  common = common->common_refpoint (s, a);
	else
	  common = s;
      }

  return common;
}

String
Grob::name () const
{
  SCM meta = get_property ("meta");
  SCM nm = scm_assq (ly_symbol2scm ("name"), meta);
  nm = (scm_is_pair (nm)) ? scm_cdr (nm) : SCM_EOL;
  return scm_is_symbol (nm) ? ly_symbol2string (nm) : this->class_name ();
}

void
Grob::set_parent (Grob *g, Axis a)
{
  dim_cache_[a].parent_ = g;
}

void
Grob::fixup_refpoint ()
{
  for (int a = X_AXIS; a < NO_AXES; a++)
    {
      Axis ax = (Axis)a;
      Grob *parent = get_parent (ax);

      if (!parent)
	continue;

      if (parent->get_system () != get_system () && get_system ())
	{
	  Grob *newparent = parent->find_broken_piece (get_system ());
	  set_parent (newparent, ax);
	}

      if (Item *i = dynamic_cast<Item *> (this))
	{
	  Item *parenti = dynamic_cast<Item *> (parent);

	  if (parenti && i)
	    {
	      Direction my_dir = i->break_status_dir ();
	      if (my_dir != parenti->break_status_dir ())
		{
		  Item *newparent = parenti->find_prebroken_piece (my_dir);
		  set_parent (newparent, ax);
		}
	    }
	}
    }
}

void
Grob::warning (String s) const
{
  SCM cause = self_scm ();
  while (Grob *g = unsmob_grob (cause))
    cause = g->get_property ("cause");

  if (Music *m = unsmob_music (cause))
    m->origin ()->warning (s);
  else
    ::warning (s);
}

void
Grob::programming_error (String s) const
{
  SCM cause = self_scm ();
  while (Grob *g = unsmob_grob (cause))
    cause = g->get_property ("cause");

  s = _f ("programming error: %s", s);

  if (Music *m = unsmob_music (cause))
    m->origin ()->message (s);
  else
    ::message (s);
}
void
Grob::discretionary_processing ()
{
}

bool
Grob::internal_has_interface (SCM k)
{
  return scm_c_memq (k, interfaces_) != SCM_BOOL_F;
}

Grob *
Grob::get_parent (Axis a) const
{
  return dim_cache_[a].parent_;
}


ADD_INTERFACE (Grob, "grob-interface",
	       "A grob represents a piece of music notation\n"
	       "\n"
	       "All grobs have an X and Y-position on the page.  These X and Y positions\n"
	       "are stored in a relative format, so they can easily be combined by\n"
	       "stacking them, hanging one grob to the side of another, and coupling\n"
	       "them into a grouping objects.\n"
	       "\n"
	       "Each grob has a reference point (a.k.a.  parent): the position of a grob\n"
	       "is stored relative to that reference point. For example the X-reference\n"
	       "point of a staccato dot usually is the note head that it applies\n"
	       "to. When the note head is moved, the staccato dot moves along\n"
	       "automatically.\n"
	       "\n"
	       "A grob is often associated with a symbol, but some grobs do not print\n"
	       "any symbols. They take care of grouping objects. For example, there is a\n"
	       "separate grob that stacks staves vertically. The @ref{NoteCollision}\n"
	       "is also an abstract grob: it only moves around chords, but doesn't print\n"
	       "anything.\n"
	       "\n"
	       "Grobs have a properties: Scheme variables, that can be read and set. "
	       "They have two types. Immutable variables "
	       "define the default style and behavior.  They are shared between  many objects. "
	       "They can be changed using @code{\\override} and @code{\\revert}. "
	       "\n\n"
	       "Mutable properties are variables that are specific to one grob. Typically, "
	       "lists of other objects, or results from computations are stored in"
	       "mutable properties: every call to set-grob-property (or its C++ equivalent) "
	       "sets a mutable property. "
	       "\n\n"
	       "The properties @code{after-line-breaking} and @code{before-line-breaking} "
	       "are dummies that are not user-serviceable. "

	       ,

	       /* properties */
	       "X-extent "
	       "X-offset "
	       "Y-extent "
	       "Y-offset "
	       "after-line-breaking "
	       "axis-group-parent-X "
	       "axis-group-parent-Y "
	       "before-line-breaking "
	       "cause "
	       "color "
	       "extra-X-extent "
	       "extra-Y-extent "
	       "extra-offset "
	       "interfaces "
	       "layer "
	       "meta "
	       "minimum-X-extent "
	       "minimum-Y-extent "
	       "springs-and-rods "
	       "staff-symbol "
	       "stencil "
	       "transparent "
	       );

