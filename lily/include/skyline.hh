/*
  skyline.hh -- declare Skyline class.

  source file of the GNU LilyPond music typesetter

  (c) 2006 Joe Neeman <joeneeman@gmail.com>
*/

#ifndef SKYLINE_HH
#define SKYLINE_HH

#include <list>

#include "axis.hh"
#include "box.hh"
#include "interval.hh"
#include "direction.hh"
#include "std-vector.hh"
#include "smobs.hh"

struct Building
{
  Interval iv_;
  Drul_array<Real> height_;

  Real y_intercept_;
  Real slope_;

  void precompute ();
  Building (Real start, Real start_height, Real end_height, Real end);
  Building (Box const &b, Real horizon_padding, Axis a, Direction d);
  void print () const;

  Real height (Real x) const;
  Real intersection_x (Building const &other) const;
  void leading_part (Real chop);
  bool conceals_beginning (Building const &other) const;
  bool conceals (Building const &other) const;
  bool sane () const;
  Building sloped_neighbour (Real horizon_padding, Direction d) const;
};

class Skyline
{
private:
  list<Building> buildings_;
  Direction sky_;
  
  void internal_merge_skyline (list<Building>*, list<Building>*,
			       list<Building> *const result);
  void internal_build_skyline (list<Building>*, list<Building> *const result);
  bool is_legal_skyline () const;

  DECLARE_SIMPLE_SMOBS(Skyline);
public:
  Skyline ();
  Skyline (Skyline const &src);
  Skyline (Direction sky);
  Skyline (vector<Box> const &bldgs, Real horizon_padding, Axis a, Direction sky);
  Skyline (Box const &b, Real horizon_padding, Axis a, Direction sky);
  vector<Offset> to_points () const;
  void merge (Skyline const &);
  void insert (Box const &, Real horizon_padding, Axis);
  void print () const;
  void raise (Real);
  void shift (Real);
  Real distance (Skyline const &) const;
  Real height (Real airplane) const;
  Real max_height () const;
  void set_minimum_height (Real height);
};

class Skyline_pair
{
private:
  Drul_array<Skyline> skylines_;

  DECLARE_SIMPLE_SMOBS(Skyline_pair);
public:
  Skyline_pair ();
  Skyline_pair (vector<Box> const &boxes, Real horizon_padding, Axis a);
  Skyline_pair (Box const &, Real horizon_padding, Axis a);
  void raise (Real);
  void shift (Real);
  void insert (Box const &, Real horizon_padding, Axis);
  void merge (Skyline_pair const &other);
  Skyline &operator [] (Direction d);
  Skyline const &operator [] (Direction d) const;
};

#endif /* SKYLINE_HH */
