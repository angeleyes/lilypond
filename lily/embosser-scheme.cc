/*
  embosser-scheme.cc -- implement Embosser scheme bindings

  source file of the GNU LilyPond music typesetter

  (c) 2007 Ralph Little
*/

#include "embosser-output.hh"

LY_DEFINE (ly_embossing_write, "ly:embossing-write",
	   2, 0, 0, (SCM embossing, SCM filename),
	   "Write @var{embossing} to @var{filename}.")
{
  LY_ASSERT_TYPE (unsmob_embosser_output, embossing, 1);
  LY_ASSERT_TYPE (scm_is_string, filename, 2);

  unsmob_embosser_output (embossing)->write_output (ly_scm2string (filename));
  return SCM_UNSPECIFIED;
}

