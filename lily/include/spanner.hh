/*
  spanner.hh -- part of GNU LilyPond

  (c) 1996--2006 Han-Wen Nienhuys
*/

#ifndef SPANNER_HH
#define SPANNER_HH

#include "grob.hh"
#include "rod.hh"

/** A symbol which is attached between two columns. A spanner is a
    symbol which spans across several columns, so its final appearance
    can only be calculated after the breaking problem is solved.

    Examples

    * (de)crescendo
    * slur
    * beam
    * bracket

    Spanner should know about the items which it should consider:
    e.g. slurs should be steep enough to "enclose" all those items. This
    is absolutely necessary for beams, since they have to adjust the
    length of stems of notes they encompass.
*/
class Spanner : public Grob
{
  Drul_array<Item *> spanned_drul_;
  vsize break_index_;

  DECLARE_CLASSNAME(Spanner);

public:
  DECLARE_SCHEME_CALLBACK (set_spacing_rods, (SCM));

  vector<Spanner*> broken_intos_;

  vsize get_break_index () const;

  // todo: move to somewhere else.
  Real get_broken_left_end_align () const;
  void substitute_one_mutable_property (SCM sym, SCM val);
  bool fast_substitute_grob_array (SCM sym, Grob_array *);

  // TODO: make virtual and do this for Items as well.
  Interval_t<int> spanned_rank_iv ();
  void set_bound (Direction d, Grob *);
  Item *get_bound (Direction d) const;

  Spanner (SCM, Object_key const *);
  Spanner (Spanner const &, int copy_count);
  bool is_broken () const;
  void do_break ();
  Real spanner_length () const;

  static int compare (Spanner *const &, Spanner *const &);
  virtual Grob *find_broken_piece (System *) const;
  virtual void derived_mark () const;
  static bool has_interface (Grob *);
  virtual System *get_system () const;

protected:
  void set_my_columns ();
  virtual Grob *clone (int count) const;
  virtual void do_break_processing ();
};

void add_bound_item (Spanner *, Grob *);

bool spanner_less (Spanner *s1, Spanner *s2);
int broken_spanner_index (Spanner *sp);
#endif
