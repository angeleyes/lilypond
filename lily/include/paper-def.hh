/*
  paper-def.hh -- declare Paper_def

  source file of the GNU LilyPond music typesetter

  (c)  1996--2003 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/


#ifndef PAPER_DEF_HH
#define PAPER_DEF_HH


#include "lily-proto.hh"
#include "lily-guile.hh"
#include "real.hh"
#include "array.hh"
#include "interval.hh"
#include "music-output-def.hh"


/** 

  Symbols, dimensions and constants pertaining to visual output.

  This struct takes care of all kinds of symbols, dimensions and
  constants. Most of them are related to the point-size of the fonts,
  so therefore, the lookup table for symbols is also in here.

  TODO: 
  
  add support for multiple fontsizes 

  remove all utility funcs 
  

  add support for other len->wid conversions.


  Interesting variables:
  
  /// The distance between lines
  interline
  
*/
class Paper_def : public Music_output_def 
{
protected:
  VIRTUAL_COPY_CONS (Music_output_def);

public:    
  Paper_outputter* get_paper_outputter (String)  const;

  SCM font_descriptions ()const;
  virtual ~Paper_def ();
  static int score_count_;
  
  /*
    JUNKME
   */
  Real get_realvar (SCM symbol) const;
  void reinit ();
  Paper_def ();
  Paper_def (Paper_def const&);

  Interval line_dimensions_int (int) const;

  void output_settings (Paper_outputter*) const;

  Font_metric * find_font (SCM name, Real mag);
  
  // urg
  friend int yyparse (void*);
};

Paper_def * unsmob_paper (SCM x);
Font_metric *select_font (Paper_def *paper, SCM chain);

#endif // Paper_def_HH
