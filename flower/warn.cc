/*
  warn.cc -- implement warnings

  source file of the Flower Library

  (c)  1997--2003 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include <stdlib.h>
#include <stdio.h>

#include "warn.hh"


void
message (String s)
{
  fputs (s.to_str0 (), stderr);
  fflush (stderr);
}

void
warning (String s)
{
  message (_f ("warning: %s\n", s.to_str0 ()));
}

void
non_fatal_error (String s)
{
  message (_f ("error: %s\n", s.to_str0 ()));
}

void
error (String s)
{
  non_fatal_error (s);
  exit (1);
}

void
programming_error (String s)
{
  message (_f ("programming error: %s (Continuing; cross thumbs)\n",
	       s.to_str0 ()));
}

