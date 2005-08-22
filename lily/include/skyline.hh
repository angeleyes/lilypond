/*
  skyline.hh -- declare Skyline_entry and funcbs.

  source file of the GNU LilyPond music typesetter

  (c) 2002--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#ifndef SKYLINE_HH
#define SKYLINE_HH

#include "array.hh"
#include "box.hh"

struct Skyline_entry
{
  Interval width_;
  Real height_;
  Skyline_entry ();
  Skyline_entry (Interval, Real);
};

void
merge_skyline (Array<Skyline_entry> *a1, Array<Skyline_entry> const &a2,
	       Direction);
void insert_extent_into_skyline (Array<Skyline_entry> *line, Box b, Axis line_axis,
				 Direction d);
Array<Skyline_entry>
extents_to_skyline (Array<Box> const &extents, Axis a, Direction d);
Array<Skyline_entry> empty_skyline (Direction d);
void heighten_skyline (Array<Skyline_entry> *buildings, Real ground);
Real
skyline_meshing_distance (Array<Skyline_entry> const &buildings,
			  Array<Skyline_entry> const &clouds);

Real
skyline_height (Array<Skyline_entry> const &buildings,
		Real airplane, Direction sky_dir);

#endif /* SKYLINE_HH */

