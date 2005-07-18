/*
  dot-column.cc -- implement Dot_column

  source file of the GNU LilyPond music typesetter

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "dot-column.hh"

#include <cstdio>
#include <math.h>
#include <map>

#include "dots.hh"
#include "rhythmic-head.hh"
#include "staff-symbol-referencer.hh"
#include "directional-element-interface.hh"
#include "side-position-interface.hh"
#include "axis-group-interface.hh"
#include "stem.hh"
#include "pointer-group-interface.hh"

/*
  TODO: let Dot_column communicate with stem via Note_column.
*/

MAKE_SCHEME_CALLBACK (Dot_column, force_shift_callback, 2);
SCM
Dot_column::force_shift_callback (SCM element_smob, SCM axis)
{
  Grob *me = unsmob_grob (element_smob);
  (void) axis;
  assert (scm_to_int (axis) == Y_AXIS);
  me = me->get_parent (X_AXIS);

  if (!to_boolean (me->get_property ("positioning-done")))
    {
      me->set_property ("positioning-done", SCM_BOOL_T);

      do_shifts (me);
    }
  return scm_make_real (0.0);
}

MAKE_SCHEME_CALLBACK (Dot_column, side_position, 2);
SCM
Dot_column::side_position (SCM element_smob, SCM axis)
{
  Grob *me = unsmob_grob (element_smob);
  (void) axis;
  assert (scm_to_int (axis) == X_AXIS);

  Grob *stem = unsmob_grob (me->get_object ("stem"));
  if (stem
      && !Stem::get_beam (stem)
      && Stem::duration_log (stem) > 2
      && !Stem::is_invisible (stem))
    {
      /*
	trigger stem end & direction calculation.

	This will add the stem to the support if a flag collision happens.
      */
      Stem::stem_end_position (stem);
    }
  return Side_position_interface::aligned_side (element_smob, axis);
}

struct Dot_position
{
  int pos_;
  Direction dir_;
  Grob *dot_;
  bool extremal_head_;

  Dot_position ()
  {
    dot_ = 0;
    pos_ = 0;
    dir_ = CENTER;
  }
};

typedef std::map<int, Dot_position> Dot_configuration;

/*
  Value CFG according.
*/
int
dot_config_badness (Dot_configuration const &cfg)
{
  int t = 0;
  for (Dot_configuration::const_iterator i (cfg.begin ());
       i != cfg.end (); i++)
    {
      int p = i->first;
      int demerit = sqr (p - i->second.pos_) * 2;

      int dot_move_dir = sign (p - i->second.pos_);
      if (i->second.extremal_head_)
	{
	  if (i->second.dir_
	      && dot_move_dir != i->second.dir_)
	    demerit += 3;
	  else if (dot_move_dir != UP)
	    demerit += 2;
	}
      else if (dot_move_dir != UP)
	demerit += 1;

      t += demerit;
    }

  return t;
}

void
print_dot_configuration (Dot_configuration const &cfg)
{
  printf ("dotconf { ");
  for (Dot_configuration::const_iterator i (cfg.begin ());
       i != cfg.end (); i++)
    printf ("%d, ", i->first);
  printf ("} \n");
}

/*
  Shift K and following (preceding) entries up (down) as necessary to
  prevent staffline collisions if D is up (down).

  If K is in CFG, then do nothing.
*/

Dot_configuration
shift_one (Dot_configuration const &cfg,
	   int k, Direction d)
{
  Dot_configuration new_cfg;
  int offset = 0;

  if (d > 0)
    {
      for (Dot_configuration::const_iterator i (cfg.begin ());
	   i != cfg.end (); i++)
	{
	  int p = i->first;
	  if (p == k)
	    {
	      if (Staff_symbol_referencer::on_staffline (i->second.dot_, p))
		p += d;
	      else
		p += 2* d;

	      offset = 2*d;

	      new_cfg[p] = i->second;
	    }
	  else
	    {
	      if (new_cfg.find (p) == new_cfg.end ())
		{
		  offset = 0;
		}
	      new_cfg[p + offset] = i->second;
	    }
	}
    }
  else
    {
      Dot_configuration::const_iterator i (cfg.end ());
      do
	{
	  i--;

	  int p = i->first;
	  if (p == k)
	    {
	      if (Staff_symbol_referencer::on_staffline (i->second.dot_, p))
		p += d;
	      else
		p += 2* d;

	      offset = 2*d;

	      new_cfg[p] = i->second;
	    }
	  else
	    {
	      if (new_cfg.find (p) == new_cfg.end ())
		{
		  offset = 0;
		}

	      new_cfg[p + offset] = i->second;
	    }
	}
      while (i != cfg.begin ());
    }

  return new_cfg;
}

/*
  Remove the collision in CFG either by shifting up or down, whichever
  is best.
*/
void
remove_collision (Dot_configuration &cfg, int p)
{
  bool collide = cfg.find (p) != cfg.end ();

  if (collide)
    {
      Dot_configuration cfg_up = shift_one (cfg, p, UP);
      Dot_configuration cfg_down = shift_one (cfg, p, DOWN);

      int b_up = dot_config_badness (cfg_up);
      int b_down = dot_config_badness (cfg_down);

      cfg = (b_up < b_down) ? cfg_up : cfg_down;
    }
}

SCM
Dot_column::do_shifts (Grob *me)
{
  Link_array<Grob> dots
    = extract_grob_array (me, "dots");

  { /*
      Trigger note collision resolution first, since that may kill off
      dots when merging.
    */
    Grob *c = 0;
    for (int i = dots.size (); i--;)
      {
	Grob *n = dots[i]->get_parent (Y_AXIS);
	if (c)
	  c = n->common_refpoint (c, X_AXIS);
	else
	  c = n;
      }
    for (int i = dots.size (); i--;)
      {
	Grob *n = dots[i]->get_parent (Y_AXIS);
	n->relative_coordinate (c, X_AXIS);
      }
  }

  dots.sort (compare_position);
  for (int i = dots.size (); i--;)
    if (!dots[i]->is_live ())
      dots.del (i);

  Dot_configuration cfg;
  for (int i = 0;i < dots.size (); i++)
    {
      Dot_position dp;
      dp.dot_ = dots[i];

      Grob *note = dots[i]->get_parent (Y_AXIS);
      if (note)
	{
	  Grob *stem = unsmob_grob (note->get_object ("stem"));
	  if (stem)
	    dp.extremal_head_ = Stem::first_head (stem) == note;
	}

      int p = Staff_symbol_referencer::get_rounded_position (dp.dot_);
      dp.pos_ = p;

      if (dp.extremal_head_)
	dp.dir_ = to_dir (dp.dot_->get_property ("direction"));

      remove_collision (cfg, p);
      cfg[p] = dp;
      if (Staff_symbol_referencer::on_staffline (dp.dot_, p))
	remove_collision (cfg, p);
    }

  for (Dot_configuration::const_iterator i (cfg.begin ());
       i != cfg.end (); i++)
    {
      Staff_symbol_referencer::set_position (i->second.dot_, i->first);
    }

  return SCM_UNSPECIFIED;
}

void
Dot_column::add_head (Grob *me, Grob *rh)
{
  Grob *d = unsmob_grob (rh->get_object ("dot"));
  if (d)
    {
      Side_position_interface::add_support (me, rh);

      Pointer_group_interface::add_grob (me, ly_symbol2scm ("dots"), d);
      d->add_offset_callback (Dot_column::force_shift_callback_proc, Y_AXIS);
      Axis_group_interface::add_element (me, d);
    }
}

ADD_INTERFACE (Dot_column, "dot-column-interface",
	       "Groups dot objects so they form a column, and position dots so they do not "
	       "clash with staff lines ",
	       "positioning-done direction stem");

