/*
  leastsquare.hh -- part of GNU LilyPond

  (c) 1996,97 Han-Wen Nienhuys
*/

#ifndef LEASTSQUARE_HH
#define LEASTSQUARE_HH
#include "varray.hh"
#include "offset.hh"


/**
  Least squares minimisation in 2 variables.
  */
struct Least_squares {
    Array<Offset> input;
    void minimise(Real &coef, Real &offset);
    void OK() const;
};


#endif // LEASTSQUARE_HH

