/*
  spacing-basic.cc -- implement Spacing_spanner, simplistic spacing routines

  source file of the GNU LilyPond music typesetter

  (c) 2005--2006 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "spacing-spanner.hh"

#include "spacing-options.hh"
#include "moment.hh"
#include "paper-column.hh"
#include "warn.hh"
#include "pointer-group-interface.hh"
#include "system.hh"

/*
  LilyPond spaces by taking a simple-minded spacing algorithm, and
  adding subtle adjustments to that. This file does the simple-minded
  spacing routines.
*/
/*
  The one-size-fits all spacing. It doesn't take into account
  different spacing wishes from one to the next column.
*/
void
Spacing_spanner::standard_breakable_column_spacing (Grob *me, Item *l, Item *r,
						    Real *fixed, Real *space,
						    Spacing_options const *options)
{
  *fixed = 0.0;
  Direction d = LEFT;
  Drul_array<Item *> cols (l, r);

  do
    {
      if (!Paper_column::is_musical (cols[d]))
	{
	  /*
	    Tied accidentals over barlines cause problems, so lets see
	    what happens if we do this for non musical columns only.
	  */
	  Interval lext = cols[d]->extent (cols [d], X_AXIS);
	  if (!lext.is_empty ())
	    *fixed += -d * lext[-d];
	}
    }
  while (flip (&d) != LEFT);

  if (Paper_column::is_breakable (l) && Paper_column::is_breakable (r))
    {
      Moment *dt = unsmob_moment (l->get_property ("measure-length"));
      Moment mlen (1);
      if (dt)
	mlen = *dt;

      Real incr = robust_scm2double (me->get_property ("spacing-increment"), 1);

      *space = *fixed + incr * double (mlen.main_part_ / options->global_shortest_) * 0.8;
    }
  else
    {
      Moment dt = Paper_column::when_mom (r) - Paper_column::when_mom (l);

      if (dt == Moment (0, 0))
	{
	  /*
	    In this case, Staff_spacing should handle the job,
	    using dt when it is 0 is silly.
	  */
	  *space = *fixed + 0.5;
	}
      else
	{
	  bool dummy;
	  *space = *fixed + options->get_duration_space (dt.main_part_, &dummy);
	}
    }
}

Moment *
get_measure_length (Grob *column)
{
  Grob * sys = column->get_parent (X_AXIS);

  extract_grob_set (sys, "columns", cols);

  vsize col_idx = binary_search (cols, column,
				 Paper_column::less_than);

  if (col_idx == VPOS)
    {
      programming_error ( __FUNCTION__ + string (": Unknown column"));
      return 0;
    }
  
  do
    {
      if (Moment *len = unsmob_moment (cols[col_idx]->get_property ("measure-length")))
	{
	  return len;
	}
    }
  while (col_idx-- != 0);
  
  return 0;
}

Real
Spacing_spanner::note_spacing (Grob *me, Grob *lc, Grob *rc,
			       Spacing_options const *options,
			       bool *expand_only)
{
  (void) me;
  
  Moment shortest_playing_len = 0;
  SCM s = lc->get_property ("shortest-playing-duration");

  if (unsmob_moment (s))
    shortest_playing_len = *unsmob_moment (s);

  if (! shortest_playing_len.to_bool ())
    {
      programming_error ("can't find a ruling note at " + Paper_column::when_mom (lc).to_string ());
      shortest_playing_len = 1;
    }

  Moment lwhen = Paper_column::when_mom (lc);
  Moment rwhen = Paper_column::when_mom (rc);

  Moment delta_t = rwhen - lwhen;
  if (!Paper_column::is_musical (rc))
    {
      /*
	when toying with mmrests, it is possible to have musical
	column on the left and non-musical on the right, spanning
	several measures.
      */

      Moment *dt = get_measure_length (lc);
      if (dt)
	{
	  delta_t = min (delta_t, *dt);

	  /*
	    The following is an extra safety measure, such that
	    the length of a mmrest event doesn't cause havoc.
	  */
	  shortest_playing_len = min (shortest_playing_len, *dt);
	}
    }

  Real dist = 0.0;
  if (delta_t.main_part_ && !lwhen.grace_part_)
    {
      dist = options->get_duration_space (shortest_playing_len.main_part_,
					  expand_only);
      dist *= double (delta_t.main_part_ / shortest_playing_len.main_part_);
    }
  else if (delta_t.grace_part_)
    {
      /*
	Crude hack for spacing graces: we take the shortest space
	available (namely the space for the global shortest note), and
	multiply that by grace-space-factor
      */
      dist = options->get_duration_space (options->global_shortest_, expand_only) / 2.0;
      Grob *grace_spacing = unsmob_grob (lc->get_object ("grace-spacing"));
      if (grace_spacing)
	{
	  Spacing_options grace_opts;
	  grace_opts.init_from_grob (grace_spacing);

	  bool bla;
	  dist = grace_opts.get_duration_space (delta_t.grace_part_, &bla);
	}
      
    }

  return dist;
}

