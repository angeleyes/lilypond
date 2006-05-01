/*
  tie-formatting-problem.hh -- declare Tie_formatting_problem

  source file of the GNU LilyPond music typesetter

  (c) 2005--2006 Han-Wen Nienhuys <hanwen@xs4all.nl>

*/

#ifndef TIE_FORMATTING_PROBLEM_HH
#define TIE_FORMATTING_PROBLEM_HH

#include "drul-array.hh"
#include "std-vector.hh"
#include "skyline.hh"
#include "lily-proto.hh"
#include "tie-configuration.hh"
#include "tie-details.hh"

#include <map>
#include <set>

template<class T, int N>
struct Tuple
{
  T t_array[N];
  Tuple (T const *src)
  {
    for (int i = 0; i < N; i++)
      t_array[i] = src[i];
  }
};

template<class T, int N>
inline bool
operator<(Tuple<T, N> const &t1,
	  Tuple<T, N> const &t2)
{
  for (int i = 0; i < N ; i++)
    {
      if (t1.t_array[i] > t2.t_array[i])
	return false;
      if (t1.t_array[i] < t2.t_array[i])
	return true;
    }

  return false;
}


typedef map< Tuple<int,4>, Tie_configuration *> Tie_configuration_map;

struct Tie_specification
{
  int position_;
  Drul_array<Grob*> note_head_drul_;
  Drul_array<int> column_ranks_;
  
  bool has_manual_position_;
  bool has_manual_dir_;
  
  Real manual_position_;
  Direction manual_dir_;
  
  Tie_specification ();
  int column_span () const;
};

struct Tie_configuration_variation
{
  int index_;
  Tie_configuration *suggestion_;
  Tie_configuration_variation ();
};

typedef map <int, vector<Skyline_entry> > Chord_outline_map;
typedef map <int, Box> Column_extent_map;
class Tie_formatting_problem
{
  Chord_outline_map chord_outlines_;
  Column_extent_map stem_extents_;
  Column_extent_map head_extents_;
  
  set<int> dot_positions_;
  Interval dot_x_;
  vector<Tie_specification> specifications_;
  
  Tie_configuration_map possibilities_;

  Grob *x_refpoint_;

  
  Tie_configuration *get_configuration (int position, Direction dir, Drul_array<int> cols) const;
  Tie_configuration *generate_configuration (int position, Direction dir, Drul_array<int> cols) const;
  vector<Tie_configuration_variation> generate_collision_variations (Ties_configuration const &ties) const;
  vector<Tie_configuration_variation> generate_extremal_tie_variations (Ties_configuration const &ties) const;

  void score_configuration (Tie_configuration *) const;
  Real score_aptitude (Tie_configuration *, Tie_specification const &,
		       Ties_configuration *, int) const;
  void score_ties_aptitude (Ties_configuration *ties) const;
  void score_ties_configuration (Ties_configuration *ties) const;
  void set_ties_config_standard_directions (Ties_configuration *tie_configs_ptr);
  void score_ties (Ties_configuration *) const;
  
  Ties_configuration generate_base_chord_configuration ();
  Ties_configuration find_best_variation (Ties_configuration const &base,
					  vector<Tie_configuration_variation> vars);

public:
  Tie_details details_;
  void print_ties_configuration (Ties_configuration const *);

  Interval get_stem_extent (int, Axis) const; 
  Interval get_head_extent (int, Axis) const; 
  
public:
  Tie_formatting_problem ();
  ~Tie_formatting_problem ();

  Tie_specification get_tie_specification (int) const;
  Ties_configuration generate_optimal_chord_configuration ();
  Ties_configuration generate_ties_configuration (Ties_configuration const &);
  Tie_configuration find_optimal_tie_configuration (Tie_specification const &) const;

  void from_ties (vector<Grob*> const &ties);
  void from_tie (Grob *tie);
  void from_semi_ties (vector<Grob*> const &, Direction head_dir);
  void set_chord_outline (vector<Item*>, Direction);
  void set_column_chord_outline (vector<Item*>, Direction, int rank);
  void set_manual_tie_configuration (SCM);
  Interval get_attachment (Real, Drul_array<int>) const;
  Grob *common_x_refpoint () const;
};

#endif /* TIE_FORMATTING_PROBLEM_HH */
