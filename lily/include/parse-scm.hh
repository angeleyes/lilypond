/*
  lily-guile.hh encapsulate guile

  source file of the GNU LilyPond music typesetter

  (c) 2004--2006 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#ifndef PARSE_SCM_HH
#define PARSE_SCM_HH

#include "input.hh"
#include "lily-guile.hh"

extern bool parse_protect_global;

struct Parse_start
{
  char const *str;
  int nchars;
  Input start_location_;
  bool safe_;
};

SCM catch_protected_parse_body (void *);
SCM protected_ly_parse_scm (Parse_start *, bool);
SCM ly_parse_scm (char const *, int *, Input, bool);

#endif /* PARSE_SCM_HH */
