/*
  paper-line.cc -- implement Paper_line

  source file of the GNU LilyPond music typesetter

  (c) 2004 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include "paper-line.hh"
#include "ly-smobs.icc"
#include "string.hh" // to_string

#define TITLE_PENALTY -1

Paper_line::Paper_line (Offset o, SCM stencils, bool is_title)
{
  dim_ = o;
  stencils_ = stencils;
  is_title_ = is_title;
  smobify_self ();
}

Paper_line::~Paper_line ()
{
}

IMPLEMENT_SMOBS (Paper_line);
IMPLEMENT_TYPE_P (Paper_line, "ly:paper-line?");
IMPLEMENT_DEFAULT_EQUAL_P (Paper_line);

SCM
Paper_line::mark_smob (SCM s)
{
  Paper_line *line = (Paper_line*) ly_cdr (s);
  return line->stencils_;
}

int
Paper_line::print_smob (SCM s, SCM port, scm_print_state*)
{
  scm_puts ("#<Paper_line ", port);
  Paper_line *line = (Paper_line*) ly_cdr (s);
  scm_puts (to_string (line->number_).to_str0 (), port);
  if (line->is_title ())
    scm_puts (" t", port);
  scm_puts (" >", port);
  return 1;
}

Offset
Paper_line::dim () const
{
  return dim_;
}

bool
Paper_line::is_title () const
{
  return is_title_;
}

SCM
Paper_line::stencils () const
{
  return stencils_;
}

LY_DEFINE (ly_paper_line_height, "ly:paper-line-height",
	   1, 0, 0, (SCM line),
	   "Return the height of @var{line}.")
{
  Paper_line *pl = unsmob_paper_line (line);
  SCM_ASSERT_TYPE (pl, line, SCM_ARG1, __FUNCTION__, "paper-line");
  return gh_double2scm (pl->dim ()[Y_AXIS]);
}

LY_DEFINE (ly_paper_line_number, "ly:paper-line-number",
	   1, 0, 0, (SCM line),
	   "Return the number of @var{line}.")
{
  Paper_line *pl = unsmob_paper_line (line);
  SCM_ASSERT_TYPE (pl, line, SCM_ARG1, __FUNCTION__, "paper-line");
  return gh_int2scm (pl->number_);
}

LY_DEFINE (ly_paper_line_break_score, "ly:paper-line-break-score",
	   1, 0, 0, (SCM line),
	   "Return the score for breaking after @var{line}.")
{
  Paper_line *pl = unsmob_paper_line (line);
  SCM_ASSERT_TYPE (pl, line, SCM_ARG1, __FUNCTION__, "paper-line");
  return gh_int2scm (int (pl->is_title ()) * TITLE_PENALTY);
}
