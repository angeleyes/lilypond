/*
  stencil.hh -- declare Stencil

  source file of the GNU LilyPond music typesetter

  (c) 1997--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/
#ifndef STENCIL_HH
#define STENCIL_HH

#include <stdlib.h>		// size_t
#include "lily-proto.hh"
#include "box.hh"
#include "axes.hh"
#include "direction.hh"
#include "lily-guile.hh"
#include "smobs.hh"

/** a group of individually translated symbols. You can add stencils
    to the top, to the right, etc.

    It is implemented as a "tree" of scheme expressions, as in

     Expr = combine Expr Expr
              | translate Offset Expr
	      | SCHEME
	      ;

    SCHEME is a Scheme expression that --when eval'd-- produces the
    desired output.  


    Because of the way that Stencil is implemented, it is the most
    efficient to add "fresh" stencils to what you're going to build.
    
    Dimension behavior:

    Empty stencils have empty dimensions.  If add_at_edge is used to
    init the stencil, we assume that
    DIMENSIONS = (Interval (0,0),Interval (0,0)
*/
class Stencil
{
  friend SCM ly_stencil_set_extent_x (SCM, SCM, SCM);

  Offset origin_;
  Box dim_;
  SCM expr_;
  
  DECLARE_SIMPLE_SMOBS (Stencil,);  
public:
  Stencil (Box, SCM s);
  Stencil ();
  
  Offset origin () const;
  SCM smobbed_copy () const;
  SCM get_expr () const;

  /**
     Set dimensions to empty, or to (Interval (0,0),Interval (0,0) */
  void set_empty (bool);
  Stencil moved_to_edge (Axis a, Direction d, const Stencil &m, Real padding,
			 Real minimum) const;

  void add_at_edge (Axis a, Direction d, const Stencil &m, Real padding,
		    Real minimum);
  void add_stencil (Stencil const &m);
  void translate (Offset);
  void align_to (Axis a, Real x);
  void translate_axis (Real,Axis);
  
  Interval extent (Axis) const;
  Box extent_box () const;
  bool is_empty () const;

  static SCM ly_get_stencil_extent (SCM mol, SCM axis);
  static SCM ly_set_stencil_extent_x (SCM,SCM,SCM);
  static SCM ly_stencil_combined_at_edge (SCM,SCM,SCM,SCM,SCM);
};


DECLARE_UNSMOB(Stencil,stencil);
SCM fontify_atom (Font_metric const*, SCM atom);

Stencil create_stencil (SCM print);



#endif
