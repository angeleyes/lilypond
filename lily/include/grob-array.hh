/*
  grob-array.hh -- declare Grob_array

  source file of the GNU LilyPond music typesetter

  (c) 2005 Han-Wen Nienhuys <hanwen@xs4all.nl>

*/

#ifndef GROB_ARRAY_HH
#define GROB_ARRAY_HH

#include "lily-proto.hh"
#include "smobs.hh"
#include "parray.hh"

class Grob_array
{
  Link_array<Grob> grobs_;
  
  DECLARE_SIMPLE_SMOBS(Grob_array,);

public:
  Item *item (int i);
  Spanner *spanner (int i);
  Grob * grob (int i);
  int size () const;
  bool is_empty () const;
  void clear ();  
  void add (Grob *);
  void set_array (Link_array<Grob> const &src);
  Link_array<Grob> &array_reference ();
  Link_array<Grob> const &array () const;
  static SCM make_array ();
};

DECLARE_UNSMOB (Grob_array, grob_array);

Link_array<Grob> const &ly_scm2link_array (SCM x);
SCM grob_list_to_grob_array (SCM lst);


#endif /* GROB_ARRAY_HH */

