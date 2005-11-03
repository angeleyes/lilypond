/*
  warn.cc -- implement warnings

  source file of the Flower Library

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "warn.hh"

#include <cstdlib>
#include <cstdio>
using namespace std;

/* Is progress indication at NEWLINE?  */
static bool progress_newline = true;

/* Display user information that is not a full message.  */
void
progress_indication (String s)
{
  /* Test if all silly progress_indication ("\n") can be dropped now.  */
  if (s == "\n")
    return;

  fputs (s.to_str0 (), stderr);
  fflush (stderr);
  if (s.length ())
    progress_newline = s[s.length () - 1] == '\n';
}

/* Display a single user message.  Always starts on a new line.  */
void
message (String s)
{
  if (!progress_newline)
    fputc ('\n', stderr);
  progress_indication (s);
}

/* Display a warning message.  Always starts on a new line.  */
void
warning (String s)
{
  message (_f ("warning: %s", s.to_str0 ()) + "\n");
}

void
non_fatal_error (String s)
{
  message (_f ("error: %s", s.to_str0 ()) + "\n");
}

/* Display an error message.  Always starts on a new line.  */
void
error (String s)
{
  non_fatal_error (s);
  exit (1);
}

void
programming_error (String s)
{
  message (_f ("programming error: %s", s) + "\n");
  message (_ ("continuing, cross fingers") + "\n");
}

