/* 
  scale.hh -- declare Scale
  
  source file of the GNU LilyPond music typesetter
  
  (c) 2006--2007 Han-Wen Nienhuys <hanwen@lilypond.org>
  2008 Rune Zedeler <rz@daimi.au.dk>
  
*/

#ifndef SCALE_HH
#define SCALE_HH

#include "smobs.hh"
#include "rational.hh"
#include "std-vector.hh"

struct Scale
{
  vector<Rational> step_tones_;
  vector<int> step_order_;
  Scale ();
  Scale (Scale const&);
  DECLARE_SMOBS (Scale);
public:
  Rational pitch_at_step(int notename);
  Rational pitch_at_step(int octave, int notename);
  /* orders the steps according to step_order_ */
  int order_steps(int stepa, int stepb);
};

extern Scale *default_global_scale;

#endif /* SCALE_HH */


