/*
  book-scheme.cc -- implement Book bindings

  source file of the GNU LilyPond music typesetter

  (c) 2004--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "book.hh"

#include "output-def.hh"
#include "score.hh"
#include "paper-book.hh"
#include "ly-module.hh"

LY_DEFINE (ly_make_book, "ly:make-book",
	   2, 0, 1, (SCM paper, SCM header, SCM scores),
	   "Make a \\book of @var{paper} and @var{header} (which may be #f as well)  "
	   "containing @code{\\scores}.")
{
  Output_def *odef = unsmob_output_def (paper);
  LY_ASSERT_SMOB (Output_def, paper, 1);

  Book *book = new Book;
  book->paper_ = odef;

  if (ly_is_module (header))
    book->header_ = header;

  book->scores_ = scm_append (scm_list_2 (scores, book->scores_));

  SCM x = book->self_scm ();
  book->unprotect ();
  return x;
}

LY_DEFINE (ly_book_process, "ly:book-process",
	   4, 0, 0, (SCM book_smob,
		     SCM default_paper,
		     SCM default_layout,
		     SCM output),
	   "Print book. @var{output} is passed to the backend unchanged. "
	   "Eg. it may be "
	   "a string (for file based outputs) or a socket (for network based "
	   "output).")
{
  Book *book = unsmob_book (book_smob);

  LY_ASSERT_SMOB (Book, book_smob, 1);
  LY_ASSERT_SMOB (Output_def, default_paper, 2);
  LY_ASSERT_SMOB (Output_def, default_layout, 3);

  Paper_book *pb = book->process (unsmob_output_def (default_paper),
				  unsmob_output_def (default_layout));
  if (pb)
    {
      pb->output (output);
      pb->unprotect ();
    }

  return SCM_UNSPECIFIED;
}


LY_DEFINE (ly_book_process_to_systems, "ly:book-process-to-systems",
	   4, 0, 0, (SCM book_smob,
		     SCM default_paper,
		     SCM default_layout,
		     SCM output),
	   "Print book. @var{output} is passed to the backend unchanged. "
	   "Eg. it may be "
	   "a string (for file based outputs) or a socket (for network based "
	   "output).")
{
  LY_ASSERT_SMOB (Book, book_smob, 1);
  LY_ASSERT_SMOB (Output_def, default_paper, 2);
  LY_ASSERT_SMOB (Output_def, default_layout, 3);

  Book *book = unsmob_book (book_smob); 

  Paper_book *pb = book->process (unsmob_output_def (default_paper),
				  unsmob_output_def (default_layout));
  if (pb)
    {
      pb->classic_output (output);
      pb->unprotect ();
    }

  return SCM_UNSPECIFIED;
}
