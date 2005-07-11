/*
  book.hh -- declare Book

  source file of the GNU LilyPond music typesetter

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#ifndef BOOK_HH
#define BOOK_HH

#include "input.hh"
#include "lily-proto.hh"
#include "parray.hh"
#include "object-key.hh"
#include "string.hh"

class Book : public Input
{
  DECLARE_SMOBS (Book, foo);

public:
  String user_key_;
  SCM header_;
  Output_def *paper_;
  SCM scores_;

  Book ();
  void add_score (SCM);
  Paper_book *process (Output_def *def_paper,
		       Output_def *def_layout);
  void set_keys ();
};

DECLARE_UNSMOB (Book, book);

#endif /* BOOK_HH */
