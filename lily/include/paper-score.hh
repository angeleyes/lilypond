/*
  paper-score.hh -- declare Paper_score

  source file of the GNU LilyPond music typesetter

  (c) 1996--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/


#ifndef P_SCORE_HH
#define P_SCORE_HH

#include "column-x-positions.hh"
#include "parray.hh"
#include "lily-proto.hh"
#include "music-output.hh"
#include "lily-guile.hh"
#include "protected-scm.hh"

/** all stuff which goes onto paper. notes, signs, symbols in a score
     #Paper_score# contains the items, the columns.
    
    */
class Paper_score : public Music_output
{
  Protected_scm main_smob_;
public:
  Paper_def *paper_;

  Paper_outputter *outputter_;  
  System *system_;
  
  Paper_score ();

  /**
    @return index of argument.
    */
  int find_col_idx (Paper_column const *) const;

  Link_array<Item> broken_col_range (Item const*,Item const*) const;
  void typeset_line (System*);
    
protected:
    /* MAIN ROUTINES */
  virtual void process (String);
private:
  /// before calc_breaking
  void preprocess ();
  void calc_idealspacing ();
  /// calculate where the lines are to be broken, and use results
  Array<Column_x_positions> calc_breaking ();

  /// after calc_breaking
  void postprocess ();
  Paper_score (Paper_score const &);
};

#endif
