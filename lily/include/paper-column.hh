/*
  paper-column.hh -- declare  Paper_column

  source file of the GNU LilyPond music typesetter

  (c) 1997--2006 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#ifndef PAPER_COLUMN_HH
#define PAPER_COLUMN_HH

#include "item.hh"
#include "rod.hh"

class Paper_column : public Item
{
  int rank_;
  /// if lines are broken then this column is in #line#
  System *system_;

  // ugh: friend declarations.
  friend void set_loose_columns (System *which, Column_x_positions const *posns);
  friend class System;
public:
  Paper_column (SCM, Object_key const *);
  Paper_column (Paper_column const &, int count);

  virtual Grob *clone (int count) const;
  virtual void do_break_processing ();
  virtual Paper_column *get_column () const;
  virtual System *get_system () const;

  int get_rank () const { return rank_; }
  void set_rank (int);

  DECLARE_SCHEME_CALLBACK (print, (SCM));
  DECLARE_SCHEME_CALLBACK (before_line_breaking, (SCM));

  static bool has_interface (Grob *);
  static int get_rank (Grob *);
  static bool is_musical (Grob *);
  static Moment when_mom (Grob *);
  static bool is_used (Grob *);
};

#endif // PAPER_COLUMN_HH

