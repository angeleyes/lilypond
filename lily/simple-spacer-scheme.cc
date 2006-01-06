/*
  simple-spacer-scheme.cc -- implement Simple_spacer

  source file of the GNU LilyPond music typesetter

  (c) 2005--2006 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include <cstdio>

#include "paper-column.hh"
#include "spring.hh"
#include "warn.hh"
#include "simple-spacer.hh"

LY_DEFINE (ly_solve_spring_rod_problem, "ly:solve-spring-rod-problem",
	   4, 1, 0, (SCM springs, SCM rods, SCM length, SCM ragged),
	   "Solve a spring and rod problem for @var{count} objects, that "
	   "are connected by @var{count-1} springs, and an arbitrary number of rods "
	   "Springs have the format (ideal, hooke) and rods (idx1, idx2, distance) "
	   "@var{length} is a number, @var{ragged} a boolean "
	   "Return: a list containing the force (positive for stretching, "
	   "negative for compressing and #f for non-satisfied constraints) "
	   "followed by the @var{spring-count}+1 positions of the objects. ")
{
  int len = scm_ilength (springs);
  if (len == 0)
    return scm_list_2 (scm_from_double (0.0), scm_from_double (0.0));

  SCM_ASSERT_TYPE (len >= 0, springs, SCM_ARG1, __FUNCTION__, "list of springs");
  SCM_ASSERT_TYPE (scm_ilength (rods) >= 0, rods, SCM_ARG2, __FUNCTION__, "list of rods");
  SCM_ASSERT_TYPE (scm_is_number (length) || length == SCM_BOOL_F,
		   length, SCM_ARG3, __FUNCTION__, "number or #f");

  bool is_ragged = ragged == SCM_BOOL_T;
  Simple_spacer spacer;
  for (SCM s = springs; scm_is_pair (s); s = scm_cdr (s))
    {
      Real ideal = scm_to_double (scm_caar (s));
      Real hooke = scm_to_double (scm_cadar (s));

      spacer.add_spring (ideal, 1 / hooke);
    }

  for (SCM s = rods; scm_is_pair (s); s = scm_cdr (s))
    {
      SCM entry = scm_car (s);
      int l = scm_to_int (scm_car (entry));
      int r = scm_to_int (scm_cadr (entry));
      entry = scm_cddr (entry);

      Real distance = scm_to_double (scm_car (entry));
      spacer.add_rod (l, r, distance);
    }

  spacer.line_len_ = scm_to_double (length);

  if (is_ragged)
    spacer.my_solve_natural_len ();
  else
    spacer.my_solve_linelen ();

  Array<Real> posns;
  posns.push (0.0);
  for (int i = 0; i < spacer.springs_.size (); i++)
    {
      Real l = spacer.springs_[i].length ((is_ragged) ? 0.0 : spacer.force_);
      posns.push (posns.top () + l);
    }

  SCM force_return = SCM_BOOL_F;
  if (!isinf (spacer.force_)
      && (spacer.is_active () || is_ragged))
    force_return = scm_from_double (spacer.force_);

  if (is_ragged
      && posns.top () > spacer.line_len_)
    force_return = SCM_BOOL_F;

  SCM retval = SCM_EOL;
  for (int i = posns.size (); i--;)
    retval = scm_cons (scm_from_double (posns[i]), retval);

  retval = scm_cons (force_return, retval);
  return retval;
}
