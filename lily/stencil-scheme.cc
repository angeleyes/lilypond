/*
  stencil-scheme.cc -- implement Stencil scheme accessors

  source file of the GNU LilyPond music typesetter

  (c) 1997--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "font-metric.hh"
#include "stencil.hh"

/*
  TODO: naming add/combine.
 */
/*
  UMGH. junkme!
  
 */
LY_DEFINE (ly_stencil_set_extent_x,"ly:stencil-set-extent!",
	   3, 0, 0, (SCM stil, SCM axis, SCM np),
	   "Set the extent of @var{stil} "
	   "(@var{extent} must be a pair of numbers) "
	   "in @var{axis} direction (0 or 1 for x- and y-axis respectively).")
{
  Stencil *s = unsmob_stencil (stil);
  SCM_ASSERT_TYPE (s, stil, SCM_ARG1, __FUNCTION__, "stencil");
  SCM_ASSERT_TYPE (is_axis (axis), axis, SCM_ARG2, __FUNCTION__, "axis");
  SCM_ASSERT_TYPE (is_number_pair (np), np, SCM_ARG3, __FUNCTION__, "number pair");

  Interval iv = ly_scm2interval (np);
  s->dim_[Axis (gh_scm2int (axis))] = iv;

  return SCM_UNDEFINED;
}

LY_DEFINE (ly_translate_stencil_axis, "ly:stencil-translate-axis",
	   3, 0, 0, (SCM stil, SCM amount, SCM axis),
	   "Return a copy of @var{stil} but translated by @var{amount} in @var{axis} direction.")
{
  Stencil *s = unsmob_stencil (stil);
  SCM_ASSERT_TYPE (s, stil, SCM_ARG1, __FUNCTION__, "stencil");
  SCM_ASSERT_TYPE (gh_number_p (amount), amount, SCM_ARG2, __FUNCTION__, "number pair");
  SCM_ASSERT_TYPE (is_axis (axis), axis, SCM_ARG3, __FUNCTION__, "axis");

  SCM new_s = s->smobbed_copy ();
  Stencil *q = unsmob_stencil (new_s);
  q->translate_axis (gh_scm2double (amount), Axis (gh_scm2int (axis)));
  return new_s;

}

LY_DEFINE (ly_translate_stencil,"ly:stencil-translate",
	   2, 0, 0, (SCM stil, SCM offset),
	   "Return a @var{stil}, "
	   "but translated by @var{offset} (a pair of numbers).")
{
  Stencil *s = unsmob_stencil (stil);
  SCM_ASSERT_TYPE (s, stil, SCM_ARG1, __FUNCTION__, "stencil");
  SCM_ASSERT_TYPE (is_number_pair (offset), offset, SCM_ARG2, __FUNCTION__, "number pair");
  Offset o = ly_scm2offset (offset);

  SCM new_s = s->smobbed_copy ();
  Stencil *q =unsmob_stencil (new_s);
  q->translate (o);
  return new_s;
}

LY_DEFINE (ly_stencil_get_expr, "ly:stencil-get-expr",
	   1 , 0, 0, (SCM stil),
	   "Return the expression of @var{stil}.")
{
  Stencil *s = unsmob_stencil (stil);
  SCM_ASSERT_TYPE (s, stil, SCM_ARG1, __FUNCTION__, "stencil");
  return s->get_expr ();
}

LY_DEFINE (ly_stencil_get_extent,
	   "ly:stencil-extent", 2 , 0, 0,  (SCM stil, SCM axis),
	   "Return a pair of numbers signifying the extent of @var{stil} in "
	   "@var{axis} direction (0 or 1 for x and y axis respectively).")
{
  Stencil *s = unsmob_stencil (stil);
  SCM_ASSERT_TYPE (s, stil, SCM_ARG1, __FUNCTION__, "stencil");
  SCM_ASSERT_TYPE (is_axis (axis), axis, SCM_ARG2, __FUNCTION__, "axis");

  return ly_interval2scm (s->extent (Axis (gh_scm2int (axis))));
}

LY_DEFINE (ly_stencil_moved_to_edge, "ly:stencil-moved-to-edge",
	   4, 2, 0,  (SCM first, SCM axis, SCM direction,
		      SCM second,
		      SCM padding,
		      SCM minimum),
	   "Similar to @code{ly:stencil-combine-edge}, but returns "
	   "@var{second} positioned to be next to @var{first}. ")
{
  /*
    C&P from combine-at-edge.
   */
  Stencil *s1 = unsmob_stencil (first);
  Stencil *s2 = unsmob_stencil (second);
  Stencil first_stencil;

  SCM_ASSERT_TYPE (is_axis (axis), axis, SCM_ARG3, __FUNCTION__, "axis");
  SCM_ASSERT_TYPE (is_direction (direction), direction, SCM_ARG4, __FUNCTION__, "dir");

  Real p = 0.0;
  if (padding != SCM_UNDEFINED)
    {
      SCM_ASSERT_TYPE (gh_number_p (padding), padding, SCM_ARG5, __FUNCTION__, "number");
      p = gh_scm2double (padding);
    }
  Real m = 0.0;
  if (minimum != SCM_UNDEFINED)
    {
      SCM_ASSERT_TYPE (gh_number_p (minimum), minimum, SCM_ARG6, __FUNCTION__, "number");
      m = gh_scm2double (minimum);
    }

  if (s1)
    first_stencil = *s1;

  if (s2)
    return first_stencil.moved_to_edge (Axis (gh_scm2int (axis)),
					Direction (gh_scm2int (direction)),
					*s2, p, m).smobbed_copy ();
  else
    return Stencil().smobbed_copy ();
}


  
LY_DEFINE (ly_stencil_combine_at_edge, "ly:stencil-combine-at-edge",
	   4, 2, 0,  (SCM first, SCM axis, SCM direction,
		      SCM second,
		      SCM padding,
		      SCM minimum),
	   "Construct a stencil by putting @var{second} next to @var{first}. "
	   "@var{axis} can be 0 (x-axis) or 1 (y-axis), "
	   "@var{direction} can be -1 (left or down) or 1 (right or up). "
	   "The stencils are juxtaposed with  @var{padding} as extra space. "
	   "If this puts the reference points closer than @var{minimum}, "
	   "they are moved by the latter amount.")
{
  Stencil *s1 = unsmob_stencil (first);
  Stencil *s2 = unsmob_stencil (second);
  Stencil result;

  SCM_ASSERT_TYPE (is_axis (axis), axis, SCM_ARG3, __FUNCTION__, "axis");
  SCM_ASSERT_TYPE (is_direction (direction), direction, SCM_ARG4, __FUNCTION__, "dir");

  Real p = 0.0;
  if (padding != SCM_UNDEFINED)
    {
      SCM_ASSERT_TYPE (gh_number_p (padding), padding, SCM_ARG5, __FUNCTION__, "number");
      p = gh_scm2double (padding);
    }
  Real m = 0.0;
  if (minimum != SCM_UNDEFINED)
    {
      SCM_ASSERT_TYPE (gh_number_p (minimum), minimum, SCM_ARG6, __FUNCTION__, "number");
      m = gh_scm2double (minimum);
    }

  if (s1)
    result = *s1;
  if (s2)
    result.add_at_edge (Axis (gh_scm2int (axis)),
			Direction (gh_scm2int (direction)), *s2, p, m);

  return result.smobbed_copy ();
}

LY_DEFINE (ly_stencil_add ,
	   "ly:stencil-add", 0, 0, 1, (SCM args),
	   "Combine stencils. Takes any number of arguments.")
{
#define FUNC_NAME __FUNCTION__
  SCM_VALIDATE_REST_ARGUMENT (args);

  Stencil result;

  while (!SCM_NULLP (args))
    {
      Stencil *s = unsmob_stencil (gh_car (args));
      if (!s)
	SCM_ASSERT_TYPE (s, gh_car (args), SCM_ARGn, __FUNCTION__, "Stencil");

      result.add_stencil (*s);
      args = gh_cdr (args);
    }

  return result.smobbed_copy ();
}

LY_DEFINE (ly_make_stencil,
	   "ly:make-stencil", 3, 0, 0,  (SCM expr, SCM xext, SCM yext),
	   " \n"
	   "Stencils are a device independent output expressions."
	   "They carry two pieces of information: \n\n"
	   "1: a specification of how to print this object. "
	   "This specification is processed by the output backends, "
	   " for example @file{scm/output-tex.scm}.\n\n"
	   "2: the vertical and horizontal extents of the object.\n\n")
{
  SCM_ASSERT_TYPE (is_number_pair (xext), xext, SCM_ARG2, __FUNCTION__, "number pair");
  SCM_ASSERT_TYPE (is_number_pair (yext), yext, SCM_ARG3, __FUNCTION__, "number pair");

  Box b (ly_scm2interval (xext), ly_scm2interval (yext));
  Stencil s (b, expr);
  return s.smobbed_copy ();
}

SCM
fontify_atom (Font_metric const *met, SCM f)
{
  if (f == SCM_EOL)
    return f;
  else
    return  scm_list_n (ly_symbol2scm ("fontify"),
			ly_quote_scm (met->description_), f, SCM_UNDEFINED);
}

LY_DEFINE (ly_fontify_atom,"ly:fontify-atom", 2, 0, 0,
	   (SCM met, SCM f),
	   "Add a font selection command for the font metric @var{met} "
	   "to @var{f}.")
{
  SCM_ASSERT_TYPE (unsmob_metrics (met), met, SCM_ARG1, __FUNCTION__, "font metric");

  return fontify_atom (unsmob_metrics (met), f);
}

LY_DEFINE (ly_align_to_x, "ly:stencil-align-to!",
	   3, 0, 0, (SCM stil, SCM axis, SCM dir),
	   "Align @var{stil} using its own extents. "
	   "@var{dir} is a number -1, 1 are left and right respectively. "
	   "Other values are interpolated (so 0 means the center. ")
{
  SCM_ASSERT_TYPE (unsmob_stencil (stil), stil, SCM_ARG1, __FUNCTION__, "stencil");
  SCM_ASSERT_TYPE (is_axis (axis), axis, SCM_ARG2, __FUNCTION__, "axis");
  SCM_ASSERT_TYPE (gh_number_p (dir), dir, SCM_ARG3, __FUNCTION__, "number");

  unsmob_stencil (stil)->align_to ((Axis)gh_scm2int (axis),
				   gh_scm2double (dir));
  return SCM_UNDEFINED;
}
