/*
    some 2D geometrical concepts
*/

#ifndef BOXES_HH
#define BOXES_HH

#include "fproto.hh"
#include "real.hh"
#include "interval.hh"
#include "offset.hh"
#include "axes.hh"

struct Box {
    Interval interval_a_[NO_AXES];
    
    Interval &x() {return interval_a_[X_AXIS]; }
    Interval &y(){ return interval_a_[Y_AXIS]; }
    Interval x()const{ return interval_a_[X_AXIS]; }
    Interval y()const{return interval_a_[Y_AXIS]; }
    Interval operator[](Axis a ) {
	return interval_a_[a];
    }
    
    void translate(Offset o) {
	x().translate(o.x());
	y().translate(o.y());
    }

    /// smallest box enclosing #b#
    void unite(Box b) {
	x().unite(b.x());
	y().unite(b.y());
    }
    Box();
    Box(Interval ix, Interval iy);
};


#endif
