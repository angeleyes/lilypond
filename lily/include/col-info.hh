/*
  col-info.hh -- declare 

  source file of the GNU LilyPond music typesetter

  (c) 1997 Han-Wen Nienhuys <hanwen@stack.nl>
*/


#ifndef COL_INFO_HH
#define COL_INFO_HH

#include "lily-proto.hh"
#include "pointer.hh"
#include "interval.hh"

/// helper struct for #Spacing_problem#
struct Colinfo {
    PCol *pcol_l_;
    P<Real> fixpos_p_;
    Interval width;
    int rank_i_;
    /// did some tricks to make this column come out.
    bool ugh_b_;		
    /* *************** */
    Colinfo();
    Colinfo(PCol *,Real const *);

    void print() const;
    bool fixed() const { return fixpos_p_.get_C();}
    Real fixed_position()const { return *fixpos_p_; }
    Real minright() const { return width.right; }
    Real minleft() const { return -width.left; }
};

#endif // COL_INFO_HH
