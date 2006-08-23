/*
  optimal-page-breaking.hh -- declare a page-breaker that
  will break pages in such a way that both horizontal and
  vertical spacing will be acceptable

  source file of the GNU LilyPond music typesetter

  (c) 2006 Joe Neeman <joeneeman@gmail.com>
*/

#ifndef OPTIMAL_PAGE_BREAKING_HH
#define OPTIMAL_PAGE_BREAKING_HH

#include "page-breaking.hh"
#include "page-spacing.hh"

class Optimal_page_breaking: public Page_breaking
{
public:
  virtual SCM solve ();

  Optimal_page_breaking (Paper_book *pb);
  virtual ~Optimal_page_breaking ();

private:
  Spacing_result try_page_spacing (Line_division const&);
};

#endif /* OPTIMAL_PAGE_BREAKING_HH */
