/*
  page-layout-problem.cc -- space systems nicely on a page. If systems can
  be stretched, do that too.

  source file of the GNU LilyPond music typesetter

  (c) 2009 Joe Neeman <joeneeman@gmail.com>
*/

#include "page-layout-problem.hh"

#include "align-interface.hh"
#include "international.hh"
#include "item.hh"
#include "output-def.hh"
#include "paper-book.hh"
#include "pointer-group-interface.hh"
#include "prob.hh"
#include "skyline-pair.hh"
#include "system.hh"

Page_layout_problem::Page_layout_problem (Paper_book *pb, SCM page_scm, SCM systems)
  : bottom_skyline_ (DOWN)
{
  Prob *page = unsmob_prob (page_scm);
  Stencil *head = unsmob_stencil (page->get_property ("head-stencil"));
  Stencil *foot = unsmob_stencil (page->get_property ("foot-stencil"));

  header_height_ = head ? head->extent (Y_AXIS).length () : 0;
  footer_height_ = foot ? foot->extent (Y_AXIS).length () : 0;
  page_height_ = robust_scm2double (page->get_property ("paper-height"), 100);

  // Initially, bottom_skyline_ represents the top of the page. Make
  // it solid, so that the top of the first system will be forced
  // below the top of the printable area.
  bottom_skyline_.set_minimum_height (-header_height_);

  SCM between_system_spacing = SCM_EOL;
  SCM after_title_spacing = SCM_EOL;
  SCM before_title_spacing = SCM_EOL;
  SCM between_title_spacing = SCM_EOL;

  // first_system_spacing controls the spring from the top of the printable
  // area to the first staff. It allows the user to control the offset of
  // the first staff (as opposed to the top of the first system) from the
  // top of the page. Similarly for last_system_spacing.
  SCM first_system_spacing = SCM_EOL;
  SCM last_system_spacing = SCM_EOL;
  if (pb && pb->paper_)
    {
      Output_def *paper = pb->paper_;
      between_system_spacing = paper->c_variable ("between-system-spacing");
      after_title_spacing = paper->c_variable ("after-title-spacing");
      before_title_spacing = paper->c_variable ("before-title-spacing");
      between_title_spacing = paper->c_variable ("between-title-spacing");
      last_system_spacing = paper->c_variable ("last-system-spacing");
      first_system_spacing = paper->c_variable ("first-system-spacing");
      if (scm_is_pair (systems) && unsmob_prob (scm_car (systems)))
	first_system_spacing = paper->c_variable ("first-system-title-spacing");

      // Note: the page height here does _not_ reserve space for headers and
      // footers. This is because we want to anchor the first-system-spacing
      // spring at the _top_ of the header.
      page_height_ -= robust_scm2double (paper->c_variable ("top-margin"), 0)
	+ robust_scm2double (paper->c_variable ("bottom-margin"), 0);
    }
  bool last_system_was_title = false;


  for (SCM s = systems; scm_is_pair (s); s = scm_cdr (s))
    {
      bool first = (s == systems);

      if (Grob *g = unsmob_grob (scm_car (s)))
	{
	  System *sys = dynamic_cast<System*> (g);
	  if (!sys)
	    {
	      programming_error ("got a grob for vertical spacing that wasn't a System");
	      continue;
	    }

	  SCM spec = first ? first_system_spacing
	    : (last_system_was_title ? after_title_spacing : between_system_spacing);
	  Spring spring (first ? 0 : 1, 0.0);
	  Real padding = 0.0;
	  alter_spring_from_spacing_spec (spec, &spring);
	  read_spacing_spec (spec, &padding, ly_symbol2scm ("padding"));

	  append_system (sys, spring, padding);
	  last_system_was_title = false;
	}
      else if (Prob *p = unsmob_prob (scm_car (s)))
	{
	  SCM spec = first ? first_system_spacing
	    : (last_system_was_title ? between_title_spacing : before_title_spacing);
	  Spring spring (first ? 0 : 1, 0.0);
	  Real padding = 0.0;
	  alter_spring_from_spacing_spec (spec, &spring);
	  read_spacing_spec (spec, &padding, ly_symbol2scm ("padding"));

	  append_prob (p, spring, padding);
	  last_system_was_title = true;
	}
      else
	programming_error ("got a system that was neither a Grob nor a Prob");
    }

  Spring last_spring (0, 0);
  Real last_padding = 0;
  alter_spring_from_spacing_spec (last_system_spacing, &last_spring);
  read_spacing_spec (last_system_spacing, &last_padding, ly_symbol2scm ("padding"));
  last_spring.ensure_min_distance (last_padding - bottom_skyline_.max_height () + footer_height_);
  springs_.push_back (last_spring);
}

void
Page_layout_problem::set_header_height (Real height)
{
  header_height_ = height;
}

void
Page_layout_problem::set_footer_height (Real height)
{
  footer_height_ = height;
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
Page_layout_problem::append_system (System *sys, Spring const& spring, Real padding)
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

  Skyline up_skyline (UP);
  Skyline down_skyline (DOWN);
  build_system_skyline (elts, minimum_offsets, &up_skyline, &down_skyline);

  Real minimum_distance = up_skyline.distance (bottom_skyline_) + padding;

  // If the previous system is a title, then distances should be measured
  // relative to the top of this system, not the refpoint of its first
  // staff.
  Spring spring_copy = spring;
  if (elements_.size () && elements_.back ().prob)
    {
      Real shift = -minimum_offsets[0];
      spring_copy.set_distance (spring_copy.distance () + shift);
      minimum_distance += shift;
    }
  spring_copy.ensure_min_distance (minimum_distance);
  springs_.push_back (spring_copy);

  bottom_skyline_ = down_skyline;
  elements_.push_back (Element (elts, minimum_offsets));

  // Add the springs for the VerticalAxisGroups in this system.

  // If the user has specified the offsets of the individual staves, fix the
  // springs at the given distances. Otherwise, use stretchable springs.
  SCM details = get_details (elements_.back ());
  SCM manual_dists = details_get_property (details, "alignment-distances");
  vsize last_spaceable_staff = 0;
  bool first_live_element = true;
  for (vsize i = 0; i < elts.size (); ++i)
    {
      if (elts[i]->is_live () && is_spaceable (elts[i]))
	{
	  // We don't add a spring for the first live element, since
	  // we are only adding springs _between_ staves here.
	  if (first_live_element)
	    {
	      first_live_element = false;
	      continue;
	    }

	  Spring spring (0.5, 0.0);
	  SCM spec = elts[last_spaceable_staff]->get_property ("next-staff-spacing");
	  alter_spring_from_spacing_spec (spec, &spring);

	  springs_.push_back (spring);
	  Real min_distance = minimum_offsets[last_spaceable_staff] - minimum_offsets[i];
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
	  last_spaceable_staff = i;
	}
    }
}

void
Page_layout_problem::append_prob (Prob *prob, Spring const& spring, Real padding)
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
  minimum_distance += padding;

  Spring spring_copy = spring;
  spring_copy.ensure_min_distance (minimum_distance);
  springs_.push_back (spring_copy);
  elements_.push_back (Element (prob));
}

void
Page_layout_problem::solve_rod_spring_problem (bool ragged)
{
  Simple_spacer spacer;

  for (vsize i = 0; i < springs_.size (); ++i)
    spacer.add_spring (springs_[i]);

  Real bottom_padding = 0;
  Interval first_staff_iv (0, 0);
  Interval last_staff_iv (0, 0);
  if (elements_.size ())
    {
      first_staff_iv = first_staff_extent (elements_[0]);
      last_staff_iv = last_staff_extent (elements_.back ());

      // TODO: junk bottom-space now that we have last-spring-spacing?
      // bottom-space has the flexibility that one can do it per-system.
      // NOTE: bottom-space is misnamed since it is not stretchable space.
      if (Prob *p = elements_.back ().prob)
	bottom_padding = robust_scm2double (p->get_property ("bottom-space"), 0);
      else if (elements_.back ().staves.size ())
	{
	  SCM details = get_details (elements_.back ());
	  bottom_padding = robust_scm2double (details_get_property (details, "bottom-space"), 0.0);
	}
    }

  spacer.solve (page_height_ - bottom_padding, ragged);
  solution_ = spacer.spring_positions ();
}

// The solution_ vector stores the position of every live VerticalAxisGroup
// and every title. From that information,
// 1) within each system, stretch the staves so they land at the right position
// 2) find the offset of each system (relative to the printable area of the page).
// FIXME: this function is getting too long.
SCM
Page_layout_problem::find_system_offsets ()
{
  SCM system_offsets = SCM_EOL;
  SCM *tail = &system_offsets;

  // spring_idx 0 is the top of the page. Interesting values start from 1.
  vsize spring_idx = 1;
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
	  Real first_staff_min_translation = elements_[i].min_offsets.size () ? elements_[i].min_offsets[0] : 0;
	  Real system_position = first_staff_position + first_staff_min_translation;

	  // Position the staves within this system.
	  Real translation = 0;
	  bool found_live_staff = false;
	  vector<Grob*> loose_lines;
	  vector<Real> const& min_offsets = elements_[i].min_offsets;
	  vector<Real> loose_line_min_distances;
	  Grob *last_spaceable_line = 0;
	  Real last_spaceable_line_translation = 0;
	  vsize last_live_staff = 0;
	  for (vsize staff_idx = 0; staff_idx < elements_[i].staves.size (); ++staff_idx)
	    {
	      Grob *staff = elements_[i].staves[staff_idx];

	      // Dead VerticalAxisGroups didn't participate in the
	      // rod-and-spring problem, but they still need to be
	      // translated. We translate them by the same amount as
	      // the VerticalAxisGroup directly before.  (but we don't
	      // increment spring_idx!)
	      if (is_spaceable (staff) || !staff->is_live ())
		{
		  if (staff->is_live ())
		    {
		      // this is relative to the system: negative numbers are down.
		      translation = system_position - solution_[spring_idx];
		      found_live_staff = true;
		      spring_idx++;

		      // Lay out any non-spaceable lines between this line and
		      // the last one.
		      if (loose_lines.size ())
			{
			  loose_line_min_distances.push_back (min_offsets[last_live_staff] - min_offsets[staff_idx]);
			  distribute_loose_lines (last_spaceable_line, last_spaceable_line_translation,
						  loose_lines, loose_line_min_distances,
						  staff, translation);
			  loose_lines.clear ();
			  loose_line_min_distances.clear ();
			}
		      last_spaceable_line = staff;
		      last_spaceable_line_translation = translation;
		      last_live_staff = staff_idx;
		    }

		  staff->translate_axis (translation, Y_AXIS);
		}
	      else
		{
		  loose_lines.push_back (staff);
		  // FIXME: this might be wrong if we haven't yet had a live staff.
		  loose_line_min_distances.push_back (min_offsets[last_live_staff] - min_offsets[staff_idx]);
		  last_live_staff = staff_idx;
		}
	    }

	  // Deal with loose lines that are not followed by spaceable lines.
	  if (loose_lines.size ())
	    {
	      loose_line_min_distances.push_back (0);
	      distribute_loose_lines (last_spaceable_line, last_spaceable_line_translation,
				      loose_lines, loose_line_min_distances,
				      0, 0);
	    }

	  // Corner case: even if a system has no live staves, it still takes up
	  // one spring (a system with one live staff also takes up one spring),
	  // which we need to increment past.
	  if (!found_live_staff)
	    spring_idx++;

	  *tail = scm_cons (scm_from_double (system_position), SCM_EOL);
	  tail = SCM_CDRLOC (*tail);
	}
    }

  assert (spring_idx == solution_.size () - 1);
  return system_offsets;
}

// Given two lines that are already spaced (line_before and line_after), distribute
// some unspaced lines between them. If line_before is null, the unspaced lines
// will be packed as closely as possible to line_after. If line_after is null, the
// unspaced lines will be packed as closely as possible to line_before. If both are
// null, the first loose_line will be translated to before_offset and the rest
// of the loose_lines will be packed as closely as possible to it.
//
// min_distances has one more element than loose_lines; the first element of
// min_distances contains the minimum skyline distance between line_before
// and loose_lines[0].
void
Page_layout_problem::distribute_loose_lines (Grob *line_before, Real before_offset,
					     vector<Grob*> const &loose_lines,
					     vector<Real> const &min_distances,
					     Grob *line_after, Real after_offset)
{
  vector<Real> offsets;
  if (!line_before || !line_after)
    {
      // Work out the distances of the lines relative to each other. (ie.
      // as close as possible).
      offsets.push_back (0);
      for (vsize i = 1; i < loose_lines.size (); ++i)
	{
	  Real min_dist = min_distances[i];
	  Real padding = robust_scm2double (loose_lines[i-1]->get_property ("padding"), 0);
	  offsets.push_back (-min_dist - padding);
	}

      // Work out the absolute offsets.
      Real shift = before_offset;
      if (line_before)
	shift -= min_distances[0]; // TODO: add padding here
      else if (line_after)
	// after shifting, the last loose_line should be at after_offset + min_distances.back ()
	shift = after_offset + min_distances.back () - offsets.back ();

      for (vsize i = 0; i < offsets.size (); ++i)
	offsets[i] += shift;
    }
  else
    {
      Simple_spacer spacer;
      Direction last_affinity = UP;
      for (vsize i = 0; i < loose_lines.size (); ++i)
	{
	  Direction affinity = robust_scm2dir (loose_lines[i]->get_property ("staff-affinity"), CENTER);
	  SCM spec = loose_lines[i]->get_property ("inter-staff-spacing");
	  Spring spring (1.0, 0.0);

	  alter_spring_from_spacing_spec (spec, &spring);

	  if (affinity > last_affinity)
	    {
	      warning (_ ("staff-affinities should only decrease"));
	      affinity = last_affinity;
	    }

	  if (affinity != last_affinity)
	    {
	      if (affinity == CENTER)
		{
		  Spring up_spring = spring;
		  up_spring.ensure_min_distance (min_distances[i]);

		  spacer.add_spring (up_spring);
		}
	      else if (affinity == DOWN && last_affinity == UP)
		{
		  // Insert a very flexible spring, so it doesn't mess things up too much.
		  Spring extra_spr (1.0, min_distances[i]);
		  extra_spr.set_inverse_stretch_strength (100000);
		  extra_spr.set_inverse_compress_strength (100000);
		  spacer.add_spring (extra_spr);
		}
	    }
	  if (affinity == UP)
	    spring.ensure_min_distance (min_distances[i]);
	  else
	    spring.ensure_min_distance (min_distances[i+1]);

	  spacer.add_spring (spring);
	  last_affinity = affinity;
	}

      if (last_affinity == UP)
	{
	  Spring extra_spr (1.0, min_distances.back ());
	  extra_spr.set_inverse_stretch_strength (100000);
	  extra_spr.set_inverse_compress_strength (100000);
	  spacer.add_spring (extra_spr);
	}

      // Remember: offsets are decreasing, since we're going from UP to DOWN!
      spacer.solve (before_offset - after_offset, false);

      vector<Real> solution = spacer.spring_positions ();
      for (vsize i = 1; i + 1 < solution.size (); ++i)
	offsets.push_back (before_offset - solution[i]);

      assert (offsets.size () == loose_lines.size ());
    }

  for (vsize i = 0; i < offsets.size (); ++i)
    loose_lines[i]->translate_axis (offsets[i], Y_AXIS);
}

SCM
Page_layout_problem::solution (bool ragged)
{
  solve_rod_spring_problem (ragged);
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
  if (minimum_translations.empty ())
    return;

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

bool
Page_layout_problem::is_spaceable (Grob *g)
{
  return !scm_is_number (g->get_property ("staff-affinity"));
}

bool
Page_layout_problem::read_spacing_spec (SCM spec, Real* dest, SCM sym)
{
  SCM pair = scm_sloppy_assq (sym, spec);
  if (scm_is_pair (pair) && scm_is_number (scm_cdr (pair)))
    {
      *dest = scm_to_double (scm_cdr (pair));
      return true;
    }
  return false;
}

void
Page_layout_problem::alter_spring_from_spacing_spec (SCM spec, Spring* spring)
{
  Real space;
  Real stretch;
  Real min_dist;
  if (read_spacing_spec (spec, &space, ly_symbol2scm ("space")))
    spring->set_distance (space);
  if (read_spacing_spec (spec, &min_dist, ly_symbol2scm ("minimum-distance")))
    spring->set_min_distance (min_dist);
  spring->set_default_strength ();

  if (read_spacing_spec (spec, &stretch, ly_symbol2scm ("stretchability")))
    {
      spring->set_inverse_stretch_strength (stretch);
      spring->set_inverse_compress_strength (stretch);
    }
}
