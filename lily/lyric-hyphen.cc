/*
  hyphen-spanner.cc -- implement Hyphen_spanner

  source file of the GNU LilyPond music typesetter

  (c) 2003--2005 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "lyric-hyphen.hh"

#include <math.h>

#include "lookup.hh"
#include "output-def.hh"
#include "paper-column.hh"
#include "moment.hh"

MAKE_SCHEME_CALLBACK (Hyphen_spanner, print, 1)
SCM
Hyphen_spanner::print (SCM smob)
{
  Spanner *me = unsmob_spanner (smob);
  Drul_array<Item *> bounds (me->get_bound (LEFT),
			     me->get_bound (RIGHT));

  if (bounds[LEFT]->break_status_dir ()
      && Paper_column::when_mom (bounds[LEFT]) == Paper_column::when_mom (bounds[RIGHT]->get_column ()))
    return SCM_EOL;

  Grob *common = bounds[LEFT]->common_refpoint (bounds[RIGHT], X_AXIS);

  Interval span_points;
  Direction d = LEFT;
  Drul_array<bool> broken;
  do
    {
      Interval iv = bounds[d]->extent (common, X_AXIS);
      
      span_points[d] = iv.is_empty ()
	? bounds[d]->relative_coordinate (common, X_AXIS)
	: iv[-d];
    }
  while (flip (&d) != LEFT);

  Real lt = me->get_layout ()->get_dimension (ly_symbol2scm ("linethickness"));
  Real th = robust_scm2double (me->get_property ("thickness"), 1) * lt;
  Real h = robust_scm2double (me->get_property ("height"), 0.5);

  // interval?

  Real dash_period = robust_scm2double (me->get_property ("dash-period"), 1.0);
  Real dash_length = robust_scm2double (me->get_property ("length"), .5);
  Real padding = robust_scm2double (me->get_property ("padding"), 0.1);
  
  if (dash_period < dash_length)
    dash_period = 1.5 * dash_length;

  
  Real l = span_points.length ();

  int n = int (ceil (l / dash_period - 0.5));
  if (n <= 0)
    n = 1;

  if (l < dash_length + 2*padding
      && !bounds[RIGHT]->break_status_dir ())
    {
      Real minimum_length = robust_scm2double (me->get_property ("minimum-length"), .3);
      dash_length = (l - 2*padding) >? minimum_length;
    }
  
  Real space_left = l - dash_length - (n - 1) * dash_period;

  /*
    If there is not enough space, the hyphen should disappear, but not
    at the end of the line.
  */
  if (space_left < 0.0
      && !bounds[RIGHT]->break_status_dir ())
    return SCM_EOL;

  space_left = space_left >? 0.0;

  Box b (Interval (0, dash_length), Interval (h, h + th));
  Stencil dash_mol (Lookup::round_filled_box (b, 0.8 * lt));

  Stencil total;
  for (int i = 0; i < n; i++)
    {
      Stencil m (dash_mol);
      m.translate_axis (span_points[LEFT] + i * dash_period + space_left / 2, X_AXIS);
      total.add_stencil (m);
    }

  total.translate_axis (-me->relative_coordinate (common, X_AXIS), X_AXIS);
  return total.smobbed_copy ();
}

MAKE_SCHEME_CALLBACK (Hyphen_spanner, set_spacing_rods, 1);
SCM
Hyphen_spanner::set_spacing_rods (SCM smob)
{
  Grob *me = unsmob_grob (smob);

  Rod r;
  Spanner *sp = dynamic_cast<Spanner *> (me);
  r.distance_
    = robust_scm2double (me->get_property ("minimum-length"), 0);

  if (r.distance_ > 0.0)
    {
      Real padding = robust_scm2double (me->get_property ("padding"), 0.1);
      r.distance_ += 2*padding;
      Direction d = LEFT;
      do
	{
	  r.item_drul_[d] = sp->get_bound (d);
	  if (r.item_drul_[d])
	    r.distance_ += r.item_drul_[d]->extent (r.item_drul_[d], X_AXIS)[-d];
	}
      while (flip (&d) != LEFT);

      if (r.item_drul_[LEFT]
	  && r.item_drul_[RIGHT])
	r.add_to_cols ();
    }
  
  return SCM_UNSPECIFIED;
}

ADD_INTERFACE (Hyphen_spanner, "lyric-hyphen-interface",
	       "A centred hyphen is a simple line between lyrics used to divide syllables",
	       "padding thickness height dash-period minimum-length length");

