/*   
  scaled-font-metric.hh -- declare Font_metric
  
  source file of the GNU LilyPond music typesetter
  
  (c) 1999--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  
 */

#ifndef SCALED_FONT_METRIC_HH
#define SCALED_FONT_METRIC_HH

#include "font-metric.hh"

/*
  Perhaps junk this, and move this to paper_def as interface? 
 */
struct Scaled_font_metric : public Font_metric
{
  virtual Box text_dimension (String) const;
  virtual Molecule find_by_name (String) const;
  static SCM make_scaled_font_metric (Font_metric*, Real);
  virtual int count () const;
  virtual Offset get_indexed_wxwy (int) const;
  virtual int name_to_index (String) const;
protected:
  virtual Box get_indexed_char (int)const;
  virtual Box get_ascii_char (int)const;
  Font_metric *orig_;
  Real magnification_;
  
  Scaled_font_metric (Font_metric*,Real);
};
#endif
