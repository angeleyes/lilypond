/*
  page-layout-problem.cc -- space systems nicely on a page. If systems can
  be stretched, do that too.

  source file of the GNU LilyPond music typesetter

  (c) 2009 Joe Neeman <joeneeman@gmail.com>
*/

#include "page-layout-problem.hh"

#include "align-interface.hh"
#include "item.hh"
#include "output-def.hh"
#include "paper-book.hh"
#include "pointer-group-interface.hh"
#include "prob.hh"
#include "skyline-pair.hh"
#include "system.hh"

Page_layout_problem::Page_layout_problem (Paper_book *pb, SCM systems)
  : bottom_skyline_ (DOWN)
{
  Output_def *paper = pb->paper_;
  between_system_space_ = robust_scm2double (paper->c_variable ("between-system-space"), 1);
  between_system_padding_ = robust_scm2double (paper->c_variable ("between-system-padding"), 0);
  Real after_title_space = robust_scm2double (paper->c_variable ("after-title-space"), 1);
  Real before_title_space = robust_scm2double (paper->c_variable ("after-title-space"), 1);
  Real between_title_space = robust_scm2double (paper->c_variable ("after-title-space"), 1);
  bool last_system_was_title = false;

  for (SCM s = systems; scm_is_pair (s); s = scm_cdr (s))
    {
      if (Grob *g = unsmob_grob (scm_car (s)))
	{
	  System *sys = dynamic_cast<System*> (g);
	  if (!sys)
	    {
	      programming_error ("got a grob for vertical spacing that wasn't a System");
	      continue;
	    }

	  Spring spring (last_system_was_title ? after_title_space
			                       : (between_system_space_ + between_system_padding_),
			 between_system_padding_);
	  append_system (sys, spring);
	  last_system_was_title = false;
	}
      else if (Prob *p = unsmob_prob (scm_car (s)))
	{
	  Spring spring (last_system_was_title ? between_title_space : before_title_space, 0.0);

	  SCM next_space = p->get_property ("next-space");
	  if (scm_is_number (next_space))
	    spring.set_distance (scm_to_double (next_space));

	  append_prob (p, spring);
	  last_system_was_title = true;
	}
    }
}

Grob*
Page_layout_problem::find_vertical_alignment (System *sys)
{
  extract_grob_set (sys, "elements", elts);
  for (vsize i = 0; i < elts.size (); ++i)
    if (Align_interface::has_interface (elts[i]))
      return elts[i];

  return 0;
}

void
Page_layout_problem::append_system (System *sys, Spring const& spring)
{
  Grob *align = find_vertical_alignment (sys);
  if (!align)
    {
      sys->programming_error ("no VerticalAlignment in system: can't do vertical spacing");
      return;
    }

  extract_grob_set (align, "elements", elts);
  vector<Real> minimum_offsets = Align_interface::get_minimum_translations (align, elts, Y_AXIS,
									    false, 0, 0);
  Real first_staff_translation = minimum_offsets.size () ? minimum_offsets[0] : 0;

  Skyline up_skyline (UP);
  Skyline down_skyline (DOWN);
  build_system_skyline (elts, minimum_offsets, &up_skyline, &down_skyline);

  // The first system doesn't get a spring before it.
  if (elements_.size ())
    {
      Real minimum_distance = up_skyline.distance (bottom_skyline_) + between_system_padding_;

      // If the previous system is a title, then distances should be measured
      // relative to the top of this system, not the refpoint of its first
      // staff.
      Spring spring_copy = spring;
      if (elements_.size () && elements_.back ().prob)
	{
	  Real shift = -first_staff_translation;
	  spring_copy.set_distance (spring_copy.distance () + shift);
	  minimum_distance += shift;
	}
      spring_copy.ensure_min_distance (minimum_distance);

      springs_.push_back (spring_copy);
    }
  bottom_skyline_ = down_skyline;
  elements_.push_back (Element (elts, first_staff_translation));

  // Add the springs for the VerticalAxisGroups in this system.
  Spring *default_spring_ptr = unsmob_spring (align->get_property ("default-spring"));
  Spring default_spring = default_spring_ptr ? *default_spring_ptr : Spring (0.5, 0.0);

  // FIXME: allow per-VerticalAxisGroup overriding of the springs.
  // In particular, deal with keep-fixed-while-stretching or deprecate it.

  // If the user has specified the offsets of the individual staves, fix the
  // springs at the given distances. Otherwise, use stretchable springs.
  SCM details = get_details (elements_.back ());
  SCM manual_dists = details_get_property (details, "alignment-distances");
  for (vsize i = 1; i < elts.size (); ++i)
    {
      if (elts[i]->is_live ())
	{
	  springs_.push_back (default_spring);
	  Real min_distance = minimum_offsets[i-1] - minimum_offsets[i];
	  springs_.back ().ensure_min_distance (min_distance);


	  if (scm_is_pair (manual_dists))
	    {
	      if (scm_is_number (scm_car (manual_dists)))
		{
		  Real dy = scm_to_double (scm_car (manual_dists));

		  springs_.back ().set_distance (-dy);
		  springs_.back ().set_min_distance (-dy);
		  springs_.back ().set_inverse_stretch_strength (0);
		}
	      manual_dists = scm_cdr (manual_dists);
	    }

	}
    }
}

void
Page_layout_problem::append_prob (Prob *prob, Spring const& spring)
{
  Skyline_pair *sky = Skyline_pair::unsmob (prob->get_property ("vertical-skylines"));
  Real minimum_distance = 0;
  if (sky)
    {
      minimum_distance = (*sky)[UP].distance (bottom_skyline_);
      bottom_skyline_ = (*sky)[DOWN];
    }
  else if (Stencil *sten = unsmob_stencil (prob->get_property ("stencil")))
    {
      Interval iv = sten->extent (Y_AXIS);
      minimum_distance = iv[UP] - bottom_skyline_.max_height ();

      bottom_skyline_.clear ();
      bottom_skyline_.set_minimum_height (iv[DOWN]);
    }
  minimum_distance += between_system_padding_;

  // The first system doesn't get a spring before it.
  if (elements_.size ())
    {
      Spring spring_copy = spring;
      spring_copy.ensure_min_distance (minimum_distance);
      springs_.push_back (spring_copy);
    }

  elements_.push_back (Element (prob));
}

void
Page_layout_problem::solve_rod_spring_problem (Real page_height, bool ragged)
{
  Simple_spacer spacer;

  for (vsize i = 0; i < springs_.size (); ++i)
    spacer.add_spring (springs_[i]);

  // TODO: currently, we space things so that the bottom of the last
  // system will touch the bottom of the printable area. Perhaps we
  // should add a spring to the last system so that we don't quite
  // fill the area (it might look better when spacing is very loose).
  Real bottom_padding = 0;
  Interval first_staff_iv (0, 0);
  Interval last_staff_iv (0, 0);
  if (elements_.size ())
    {
      first_staff_iv = first_staff_extent (elements_[0]);
      last_staff_iv = last_staff_extent (elements_.back ());

      // NOTE: bottom-space is misnamed since it is not stretchable space.
      if (Prob *p = elements_.back ().prob)
	bottom_padding = robust_scm2double (p->get_property ("bottom-space"), 0);
      else if (elements_.back ().staves.size ())
	{
	  SCM details = get_details (elements_.back ());
	  bottom_padding = robust_scm2double (details_get_property (details, "bottom-space"), 0.0);
	}
    }

  spacer.solve (page_height - bottom_padding + last_staff_iv[DOWN] - first_staff_iv[UP], ragged);
  solution_ = spacer.spring_positions ();

  // Ensure that the top of the top staff is at the top the space we were given.
  for (vsize i = 0; i < solution_.size (); ++i)
    solution_[i] += first_staff_iv[UP];

  // TODO: (maybe) if it doesn't fit, try again without padding.
}

// The solution_ vector stores the position of every live VerticalAxisGroup
// and every title. From that information,
// 1) within each system, stretch the staves so they land at the right position
// 2) find the offset of each system (relative to the printable area of the page).
SCM
Page_layout_problem::find_system_offsets ()
{
  SCM system_offsets = SCM_EOL;
  SCM *tail = &system_offsets;

  vsize spring_idx = 0;
  for (vsize i = 0; i < elements_.size (); ++i)
    {
      if (elements_[i].prob)
	{
	  *tail = scm_cons (scm_from_double (solution_[spring_idx]), SCM_EOL);
	  tail = SCM_CDRLOC (*tail);
	  spring_idx++;
	}
      else
	{
	  // Getting this signs right here is a little tricky. The configuration
	  // we return has zero at the top of the page and positive numbers further
	  // down, as does the solution_ vector.  Within a staff, however, positive
	  // numbers are up.
	  // TODO: perhaps change the way the page 'configuration variable works so
	  // that it is consistent with the usual up/down sign conventions in
	  // Lilypond. Then this would be less confusing.

	  // These two positions are relative to the page (with positive numbers being
	  // down).
	  Real first_staff_position = solution_[spring_idx];
	  Real system_position = first_staff_position + elements_[i].first_staff_min_translation;

	  // Position the staves within this system.
	  Real translation = 0;
	  for (vsize staff_idx = 0; staff_idx < elements_[i].staves.size (); ++staff_idx)
	    {
	      Grob *staff = elements_[i].staves[staff_idx];

	      // Dead VerticalAxisGroups didn't participate in the
	      // rod-and-spring problem, but they still need to be
	      // translated. We translate them by the same amount as
	      // the VerticalAxisGroup directly before.  (but we don't
	      // increment spring_idx!)
	      if (staff->is_live ())
		{
		  // this is relative to the system: negative numbers are down.
		  translation = system_position - solution_[spring_idx];
		  spring_idx++;
		}
	      staff->translate_axis (translation, Y_AXIS);
	    }

	  // Corner case: even if a system has no live staves, it still takes up
	  // one spring (a system with one live staff also takes up one spring),
	  // which we need to increment past.
	  if (elements_[i].staves.empty ())
	    spring_idx++;

	  *tail = scm_cons (scm_from_double (system_position), SCM_EOL);
	  tail = SCM_CDRLOC (*tail);
	}
    }

  assert (spring_idx == solution_.size ());
  return system_offsets;
}

SCM
Page_layout_problem::solution (Real page_height, bool ragged)
{
  solve_rod_spring_problem (page_height, ragged);
  return find_system_offsets ();
}

// Build upper and lower skylines for a system. We don't yet know the positions
// of the staves within the system, so we make the skyline as conservative as
// possible. That is, for the upper skyline, we pretend that all of the staves
// in the system are packed together close to the top system; for the lower
// skyline, we pretend that all of the staves are packed together close to
// the bottom system.
//
// The upper skyline is relative to the top staff; the lower skyline is relative to
// the bottom staff.
void
Page_layout_problem::build_system_skyline (vector<Grob*> const& staves,
					   vector<Real> const& minimum_translations,
					   Skyline *up,
					   Skyline *down)
{
  assert (staves.size () == minimum_translations.size ());
  Real first_translation = minimum_translations[0];

  for (vsize i = 0; i < staves.size (); ++i)
    {
      Real dy = minimum_translations[i] - first_translation;
      Grob *g = staves[i];
      Skyline_pair *sky = Skyline_pair::unsmob (g->get_property ("vertical-skylines"));
      if (sky)
	{
	  up->raise (-dy);
	  up->merge ((*sky)[UP]);
	  up->raise (dy);

	  down->raise (-dy);
	  down->merge ((*sky)[DOWN]);

	  // The last time through, leave the down skyline at a position
	  // relative to the bottom staff.
	  if (i < staves.size () - 1)
	    down->raise (dy);
	}
    }
}

Interval
Page_layout_problem::prob_extent (Prob *p)
{
  Stencil *sten = unsmob_stencil (p->get_property ("stencil"));
  return sten ? sten->extent (Y_AXIS) : Interval (0, 0);
}

Interval
Page_layout_problem::first_staff_extent (Element const& e)
{
  if (e.prob)
    return prob_extent (e.prob);
  else if (e.staves.size ())
    return e.staves[0]->extent (e.staves[0], Y_AXIS);

  return Interval (0, 0);
}

Interval
Page_layout_problem::last_staff_extent (Element const& e)
{
  if (e.prob)
    return prob_extent (e.prob);
  else if (e.staves.size ())
    return e.staves.back ()->extent (e.staves.back (), Y_AXIS);

  return Interval (0, 0);
}

SCM
Page_layout_problem::get_details (Element const& elt)
{
  if (elt.staves.empty ())
    return SCM_EOL;

  System *sys = elt.staves.back ()->get_system ();
  Grob *left_bound = sys->get_bound (LEFT);
  return left_bound->get_property ("line-break-system-details");
}

SCM
Page_layout_problem::details_get_property (SCM details, const char* property)
{
  SCM handle = scm_assoc (ly_symbol2scm (property), details);
  return scm_is_pair (handle) ? scm_cdr (handle) : SCM_BOOL_F;
}
