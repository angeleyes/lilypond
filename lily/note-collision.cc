/*
  collision.cc -- implement Collision

  source file of the GNU LilyPond music typesetter

  (c)  1997--2003 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "warn.hh"
#include "note-collision.hh"
#include "note-column.hh"
#include "note-head.hh"
#include "rhythmic-head.hh"
#include "paper-def.hh"
#include "axis-group-interface.hh"
#include "item.hh"
#include "stem.hh"
#include "side-position-interface.hh"
#include "dot-column.hh"

MAKE_SCHEME_CALLBACK (Note_collision_interface,force_shift_callback,2);

SCM
Note_collision_interface::force_shift_callback (SCM element_smob, SCM axis)
{
  Grob *me = unsmob_grob (element_smob);
  Axis a = (Axis) gh_scm2int (axis);
  assert (a == X_AXIS);
  
   me = me->get_parent (a);

   if (! to_boolean (me->get_grob_property ("positioning-done")))
    {
      me->set_grob_property ("positioning-done", SCM_BOOL_T);
      do_shifts (me);
    }
  
  return gh_double2scm (0.0);
}


void
check_meshing_chords (Grob *me,
		      Drul_array< Array < Real > > *offsets,
		      Drul_array< Array < Slice > > const &extents,
		      Drul_array<Link_array<Grob> > const &clash_groups)
	
{
  if (!extents[UP].size () || ! extents[DOWN].size ())
    return;
  
  Grob *cu = clash_groups[UP][0];
  Grob *cd = clash_groups[DOWN][0];

  /* Every note column should have a stem, but avoid a crash. */
  if (!Note_column::get_stem (cu) || !Note_column::get_stem (cd))
    return;

  Grob *nu = Note_column::first_head (cu);
  Grob *nd = Note_column::first_head (cd);

  Array<int> ups = Stem::note_head_positions (Note_column::get_stem (cu));
  Array<int> dps = Stem::note_head_positions (Note_column::get_stem (cd));

  /* Too far apart to collide.  */
  if (ups[0] > dps.top () + 1)
    return; 

  // FIXME: what's this?
  bool merge_possible = (ups[0] >= dps[0]) && (ups.top () >= dps.top ());

  int upball_type = Note_head::get_balltype (nu);
  int dnball_type = Note_head::get_balltype (nd);
  
  /* Do not merge whole notes (or longer, like breve, longa, maxima).  */
  if (merge_possible && (upball_type <= 0 || dnball_type <= 0))
    merge_possible = false;

  if (merge_possible
      && Rhythmic_head::dot_count (nu) != Rhythmic_head::dot_count (nd)
      && !to_boolean (me->get_grob_property ("merge-differently-dotted")))
    merge_possible = false;

  /* Can only merge different heads if merge-differently-headed is
     set. */
  if (merge_possible
      && upball_type != dnball_type
      && !to_boolean (me->get_grob_property ("merge-differently-headed")))
    merge_possible = false;

  /* Can never merge quarter and half notes. */
  if (merge_possible
      && ((Rhythmic_head::duration_log (nu) == 1
	   && Rhythmic_head::duration_log (nd) == 2)
	  || (Rhythmic_head::duration_log (nu) == 2
	     && Rhythmic_head::duration_log (nd) == 1)))
    merge_possible = false;


  /*
    this case (distant half collide), 
    
        |
      x |
     | x
     |

   the noteheads may be closer than this case (close half collide)

       |
       |
      x 
     x
    |
    |
    
   */
  
  /* TODO: filter out the 'o's in this configuration, since they're no
  part in the collision.

     |
    x|o
    x|o
    x
    
   */
  
  bool close_half_collide = false;
  bool distant_half_collide = false;  
  bool full_collide = false;  

  int i = 0, j=0;
  while (i < ups.size () && j < dps.size ())
  {
    if (abs (ups[i] - dps[j]) == 1)
      {
	merge_possible = false;
	if (ups[i] > dps[j])
	  close_half_collide = true;
	else
	  distant_half_collide = true;
      }
    else if (ups[i]==dps[j])
      full_collide = true;
    else if (ups[i] >dps[0] && ups[i] < dps.top ())
      merge_possible = false;
    else if (dps[j] >ups[0] && dps[j] < ups.top ())
      merge_possible = false;
    
    if (ups[i] < dps[j])
      i++;
    else if (ups[i] > dps[j])
      j++;
    else
      {
	i++;
	j++;
      }
  }

  Drul_array<Real> center_note_shifts;
  center_note_shifts[LEFT] = 0.0;
  center_note_shifts[RIGHT] = 0.0;

  
  Real shift_amount = 1;

  bool touch = (ups[0] - dps.top () >= 0);
  if (touch)
    shift_amount *= -1;

  /* For full collisions, the right hand head may obscure dots, so
     make sure the dotted heads go to the right.  */
  if (Rhythmic_head::dot_count (nu) > Rhythmic_head::dot_count (nd)
      && full_collide)
    shift_amount = 1;

  if (merge_possible)
    {
      shift_amount = 0;

      /* Wipe shortest head, or head with smallest amount of dots.
	 Note: when merging different heads, dots on shortest
	 disappear. */
      
      Grob *wipe_ball = nu;
      
      if (upball_type == dnball_type)
	{
	  if (Rhythmic_head::dot_count (nd) < Rhythmic_head::dot_count (nu))
	    wipe_ball = nd;
	}
      else if (dnball_type > upball_type)
	wipe_ball = nd;

      if (wipe_ball->live ())
	{
	  wipe_ball->set_grob_property ("transparent", SCM_BOOL_T);
	  wipe_ball->set_grob_property ("molecule", SCM_EOL);

	  if (Grob *d = unsmob_grob (wipe_ball->get_grob_property ("dot")))
	    d->suicide ();
	}
    }
  /* TODO: these numbers are magic; should devise a set of grob props
     to tune this behavior.  */
  else if (close_half_collide && !touch)
    shift_amount *= 0.52;
  else if (distant_half_collide && !touch)
    shift_amount *= 0.4;
  else if (distant_half_collide || close_half_collide || full_collide)
    shift_amount *= 0.5;
  
  /* we're meshing.  */
  else if (Rhythmic_head::dot_count (nu) || Rhythmic_head::dot_count (nd))
    shift_amount *= 0.1;
  else
    shift_amount *= 0.25;

  /* For full or close half collisions, the right hand head may
     obscure dots.  Move dots to the right.  */
  if (abs (shift_amount) > 1e-6
      && Rhythmic_head::dot_count (nd) > Rhythmic_head::dot_count (nu)
      && (full_collide || close_half_collide))
    {
      Grob *d = unsmob_grob (nd->get_grob_property ("dot"));
      Grob *parent = d->get_parent (X_AXIS);
      if (Dot_column::has_interface (parent))
	Side_position_interface::add_support (parent, nu);
    }

  Direction d = UP;
  do
    {
      for (int i=0; i < clash_groups[d].size (); i++)
	(*offsets)[d][i] += d * shift_amount;
    }
  while ((flip (&d))!= UP);
}

void
Note_collision_interface::do_shifts (Grob* me)
{
  Drul_array< Link_array <Grob>  > cg = get_clash_groups (me);

  SCM autos (automatic_shift (me, cg));
  SCM hand (forced_shift (me));

  
  
  Direction d = UP;
  Real wid = 0.0;
  do
    {
      if(cg[d].size())
	{
	  Grob  *h = cg[d][0];
	  wid = Note_column::first_head(h)->extent(h,X_AXIS).length() ;
	}
    }
  
  while (flip (&d) != UP);

  
  Link_array<Grob> done;
  for (; gh_pair_p (hand); hand =ly_cdr (hand))
    {
      Grob * s = unsmob_grob (ly_caar (hand));
      Real amount = gh_scm2double (ly_cdar (hand));
      
      s->translate_axis (amount *wid, X_AXIS);
      done.push (s);
    }
  for (; gh_pair_p (autos); autos =ly_cdr (autos))
    {
      Grob * s = unsmob_grob (ly_caar (autos));
      Real amount = gh_scm2double (ly_cdar (autos));
      
      if (!done.find (s))
	s->translate_axis (amount * wid, X_AXIS);
    }
}

Drul_array< Link_array <Grob>  >
Note_collision_interface::get_clash_groups (Grob *me)
{
  Drul_array<Link_array<Grob> > clash_groups;
 
  SCM s = me->get_grob_property ("elements");
  for (; gh_pair_p (s); s = ly_cdr (s))
    {
      SCM car = ly_car (s);

      Grob * se = unsmob_grob (car);
      if (Note_column::has_interface (se))
	clash_groups[Note_column::dir (se)].push (se);
    }
  
  Direction d = UP;
  do
    {
      Link_array<Grob> & clashes (clash_groups[d]);
      clashes.sort (Note_column::shift_compare);
    }
  while ((flip (&d))!= UP);

  return clash_groups;
}

/** This complicated routine moves note columns around horizontally to
  ensure that notes don't clash.

  This should be put into Scheme.  
  */
SCM
Note_collision_interface::automatic_shift (Grob *me,
			    Drul_array< Link_array <Grob> > 
			    clash_groups)
{
  Drul_array<Array<int> > shifts;
  SCM  tups = SCM_EOL;

  
  Direction d = UP;
  do
    {
      Array<int> & shift (shifts[d]);
      Link_array<Grob> & clashes (clash_groups[d]);

      for (int i=0; i < clashes.size (); i++)
	{
	  SCM sh
	    = clashes[i]->get_grob_property ("horizontal-shift");

	  if (gh_number_p (sh))
	    shift.push (gh_scm2int (sh));
	  else
	    shift.push (0);
	}
      
      for (int i=1; i < shift.size (); i++)
	{
	  if (shift[i-1] == shift[i])
	    {
	      clashes[0]->warning (_ ("Too many clashing notecolumns.  Ignoring them."));
	      return tups;
	    }
	}
    }
  while ((flip (&d))!= UP);

  Drul_array< Array < Slice > > extents;
  Drul_array< Array < Real > > offsets;
  d = UP;
  do
    {
      for (int i=0; i < clash_groups[d].size (); i++)
	{
	  Slice s (Note_column::head_positions_interval (clash_groups[d][i]));
	  s[LEFT] --;
	  s[RIGHT]++;
	  extents[d].push (s);
	  offsets[d].push (d * 0.5 * i);
	}
    }
  while ((flip (&d))!= UP);

  /*
    do horizontal shifts of each direction 

       | 
      x||
       x||
        x|
   */
  
  do
    {
      for (int i=1; i < clash_groups[d].size (); i++)
	{
	  Slice prev =extents[d][i-1];
	  prev.intersect (extents[d][i]);
	  if (prev.length ()> 0 ||
 (extents[-d].size () && d * (extents[d][i][-d] - extents[-d][0][d]) < 0))
	    for (int j = i; j <  clash_groups[d].size (); j++)
	      offsets[d][j] += d * 0.5;
	}
    }	
  while ((flip (&d))!= UP);


  /*
    Check if chords are meshing
   */

  check_meshing_chords (me, &offsets, extents, clash_groups);
  
  do
    {
      for (int i=0; i < clash_groups[d].size (); i++)
	tups = gh_cons (gh_cons (clash_groups[d][i]->self_scm (),
				 gh_double2scm (offsets[d][i])),
			tups);
    }
  while (flip (&d) != UP);
  return tups;
}


SCM
Note_collision_interface::forced_shift (Grob *me)
{
  SCM tups = SCM_EOL;
  
  SCM s = me->get_grob_property ("elements");
  for (; gh_pair_p (s); s = ly_cdr (s))
    {
      Grob * se = unsmob_grob (ly_car (s));

      SCM force =  se->get_grob_property ("force-hshift");
      if (gh_number_p (force))
	{
	  tups = gh_cons (gh_cons (se->self_scm (), force),
			  tups);
	}
    }
  return tups;
}

void
Note_collision_interface::add_column (Grob*me,Grob* ncol)
{
  ncol->add_offset_callback (Note_collision_interface::force_shift_callback_proc, X_AXIS);
  Axis_group_interface::add_element (me, ncol);
  me->add_dependency (ncol);
}


ADD_INTERFACE (Note_collision_interface, "note-collision-interface",
  "An object that handles collisions between notes with different stem " 
"directions and horizontal shifts. Most of the interesting properties "
"are to be set in @ref{note-column-interface}: these are "
"@code{force-hshift} and @code{horizontal-shift}. ",
  "merge-differently-dotted merge-differently-headed positioning-done");
