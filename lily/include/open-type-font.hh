/*
  open-type-font.hh -- declare Open_type_font

  source file of the GNU LilyPond music typesetter

  (c) 2004--2006 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#ifndef OPEN_TYPE_FONT_HH
#define OPEN_TYPE_FONT_HH


#include "font-metric.hh"

Index_to_charcode_map make_index_to_charcode_map (FT_Face face);
void get_unicode_name (char*s, FT_ULong code);
void get_glyph_index_name (char*s, FT_ULong code);

class Open_type_font : public Font_metric
{
  /* handle to face object */
  FT_Face face_;

  SCM lily_subfonts_;
  SCM lily_character_table_;
  SCM lily_global_table_;
  SCM lily_index_to_bbox_table_;

  Index_to_charcode_map index_to_charcode_map_;
  Open_type_font (FT_Face);

  DECLARE_CLASSNAME(Open_type_font);
public:
  SCM get_subfonts () const;
  SCM get_global_table () const;
  SCM get_char_table () const;
  SCM glyph_list () const;
    
  string get_otf_table (string tag) const;
  static SCM make_otf (string);
  string font_name () const;
  ~Open_type_font ();
  Offset attachment_point (string) const;
  size_t count () const;
  Box get_indexed_char (size_t) const;
  size_t name_to_index (string) const;
  //size_t glyph_name_to_charcode (string) const;
  size_t index_to_charcode (size_t) const;
  void derived_mark () const;
  SCM sub_fonts () const;
  Real design_size () const;
};

string get_otf_table (FT_Face face, string tag);
FT_Face open_ft_face (string str);


#endif /* OPEN_TYPE_FONT_HH */
