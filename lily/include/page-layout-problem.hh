/*
  page-layout-problem.hh -- space systems nicely on a page. If systems can
  be stretched, do that too.

  source file of the GNU LilyPond music typesetter

  (c) 2009 Joe Neeman <joeneeman@gmail.com>
*/

#ifndef PAGE_LAYOUT_HH
#define PAGE_LAYOUT_HH

#include "simple-spacer.hh"
#include "skyline.hh"

class Page_layout_problem
{
public:
  Page_layout_problem (Paper_book*, SCM systems);

  SCM solution (Real page_height, bool ragged);

protected:
  void append_system (System*, Spring const&);
  void append_prob (Prob*, Spring const&);

  void solve_rod_spring_problem (Real page_height, bool ragged);
  SCM find_system_offsets ();

  static Grob* find_vertical_alignment (System*);
  static void build_system_skyline (vector<Grob*> const&, vector<Real> const&, Skyline* up, Skyline* down);

  // This is a union (in spirit).
  // Either staves must be empty or prob must be null.
  typedef struct Element {
    Prob *prob;
    vector<Grob*> staves;
    Real first_staff_min_translation;

    Element (vector<Grob*> const& a, Real r)
    {
      staves = a;
      first_staff_min_translation = r;
      prob = 0;
    }

    Element (Prob *p)
    {
      prob = p;
    }
  } Element;

  static Interval first_staff_extent (Element const&);
  static Interval last_staff_extent (Element const&);
  static Interval prob_extent (Prob*);
  static SCM get_details (Element const&);
  static SCM details_get_property (SCM details, const char*);

  vector<Spring> springs_;
  vector<Element> elements_;
  vector<Real> solution_;
  Skyline bottom_skyline_;
};

#endif /* PAGE_LAYOUT_HH */
