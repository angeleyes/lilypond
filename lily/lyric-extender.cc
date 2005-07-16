/*
  lyric-extender.cc -- implement Lyric_extender
  source file of the GNU LilyPond music typesetter

  (c) 1998--2005 Jan Nieuwenhuizen <janneke@gnu.org>
  Han-Wen Nienhuys
*/

#include "lyric-extender.hh"

#include <math.h>

#include "warn.hh"
#include "lookup.hh"
#include "paper-column.hh"
#include "output-def.hh"
#include "note-head.hh"
#include "pointer-group-interface.hh"

MAKE_SCHEME_CALLBACK (Lyric_extender, print, 1)
  SCM
Lyric_extender::print (SCM smob)
{
  Spanner *me = unsmob_spanner (smob);
  Item *left_edge = me->get_bound (LEFT);
  Item *right_text = unsmob_item (me->get_object ("next"));

  Grob *common = left_edge;

  if (right_text)
    common = common->common_refpoint (right_text, X_AXIS);

  common = common->common_refpoint (me->get_bound (RIGHT), X_AXIS);
  Real sl = me->get_layout ()->get_dimension (ly_symbol2scm ("linethickness"));

  extract_grob_set (me, "heads", heads);

  if (!heads.size ())
    return SCM_EOL;

  common = common_refpoint_of_array (heads, common, X_AXIS);

  Real left_point = 0.0;
  if (left_edge->internal_has_interface (ly_symbol2scm ("lyric-syllable-interface")))
    left_point = left_edge->extent (common, X_AXIS)[RIGHT];
  else if (heads.size ())
    left_point = heads[0]->extent (common, X_AXIS)[LEFT];
  else
    left_point = left_edge->extent (common, X_AXIS)[RIGHT];

  if (isinf (left_point))
    return SCM_EOL;

  /* It seems that short extenders are even lengthened to go past the
     note head, but haven't found a pattern in it yet. --hwn 1/1/04  */
  SCM minlen = me->get_property ("minimum-length");
  Real right_point
    = left_point + (robust_scm2double (minlen, 0));

  if (heads.size ())
    right_point = max (right_point, heads.top ()->extent (common, X_AXIS)[RIGHT]);

  Real h = sl * robust_scm2double (me->get_property ("thickness"), 0);
  Real pad = 2* h;

  if (right_text)
    right_point = min (right_point, (robust_relative_extent (right_text, common, X_AXIS)[LEFT] - pad));

  /* run to end of line. */
  if (me->get_bound (RIGHT)->break_status_dir ())
    right_point = max (right_point, (robust_relative_extent (me->get_bound (RIGHT), common, X_AXIS)[LEFT] - pad));

  left_point += pad;
  Real w = right_point - left_point;

  if (w < 1.5 * h)
    return SCM_EOL;

  Stencil mol (Lookup::round_filled_box (Box (Interval (0, w),
					      Interval (0, h)),
					 0.8 * h));
  mol.translate_axis (left_point - me->relative_coordinate (common, X_AXIS),
		      X_AXIS);
  return mol.smobbed_copy ();
}

ADD_INTERFACE (Lyric_extender, "lyric-extender-interface",
	       "The extender is a simple line at the baseline of the lyric "
	       "that helps show the length of a melissima (tied/slurred note).",
	       "next thickness heads");
