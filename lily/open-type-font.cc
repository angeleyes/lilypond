/*
  open-type-font.cc -- implement Open_type_font

  source file of the GNU LilyPond music typesetter

  (c) 2004--2006 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "open-type-font.hh"

#include <cstdio>

using namespace std;

#include <freetype/tttables.h>

#include "dimensions.hh"
#include "international.hh"
#include "modified-font-metric.hh"
#include "warn.hh"

FT_Byte *
load_table (char const *tag_str, FT_Face face, FT_ULong *length)
{
  *length = 0;
  FT_ULong tag = FT_MAKE_TAG (tag_str[0], tag_str[1], tag_str[2], tag_str[3]);

  int error_code = FT_Load_Sfnt_Table (face, tag, 0, NULL, length);
  if (!error_code)
    {
      FT_Byte *buffer = (FT_Byte *) malloc (*length);
      if (buffer == NULL)
	error (_f ("cannot allocate %lu bytes", *length));

      error_code = FT_Load_Sfnt_Table (face, tag, 0, buffer, length);
      if (error_code)
	error (_f ("cannot load font table: %s", tag_str));

      return buffer;
    }
  else
    programming_error ("Cannot find OpenType table.");

  return 0;
}

string
Open_type_font::get_otf_table (string tag) const
{
  return ::get_otf_table (face_, tag);
}

SCM
load_scheme_table (char const *tag_str, FT_Face face)
{
  FT_ULong length = 0;
  FT_Byte *buffer = load_table (tag_str, face, &length);

  SCM tab = SCM_EOL;
  if (buffer)
    {
      string contents ((char const*)buffer, length);
      contents = "(quote (" + contents + "))";

      tab = scm_c_eval_string (contents.c_str ());
      free (buffer);
    }
  return tab;
}


Open_type_font::~Open_type_font ()
{
  FT_Done_Face (face_);
}

/*
  UGH fix naming
*/
string
get_otf_table (FT_Face face, string tag)
{
  FT_ULong len;
  FT_Byte *tab = load_table (tag.c_str (), face, &len);
  string ret ((char const*) tab, len);
  free (tab);

  return ret;
}

FT_Face
open_ft_face (string str)
{
  FT_Face face;
  int error_code = FT_New_Face (freetype2_library, str.c_str (), 0, &face);

  if (error_code == FT_Err_Unknown_File_Format)
    error (_f ("unsupported font format: %s", str.c_str ()));
  else if (error_code)
    error (_f ("unknown error: %d reading font file: %s", error_code,
	       str.c_str ()));
  return face;
}

SCM
Open_type_font::make_otf (string str)
{
  FT_Face face = open_ft_face (str);
  Open_type_font *otf = new Open_type_font (face);

  return otf->self_scm ();
}

Open_type_font::Open_type_font (FT_Face face)
{
  face_ = face;
  lily_character_table_ = SCM_EOL;
  lily_global_table_ = SCM_EOL;
  lily_subfonts_ = SCM_EOL;
  lily_index_to_bbox_table_ = SCM_EOL;

  lily_character_table_ = alist_to_hashq (load_scheme_table ("LILC", face_));
  lily_global_table_ = alist_to_hashq (load_scheme_table ("LILY", face_));
  lily_subfonts_ = load_scheme_table ("LILF", face_);
  index_to_charcode_map_ = make_index_to_charcode_map (face_);

  lily_index_to_bbox_table_ = scm_c_make_hash_table (257);
}

void
Open_type_font::derived_mark () const
{
  scm_gc_mark (lily_character_table_);
  scm_gc_mark (lily_global_table_);
  scm_gc_mark (lily_subfonts_);
  scm_gc_mark (lily_index_to_bbox_table_);
}

Offset
Open_type_font::attachment_point (string glyph_name) const
{
  SCM sym = ly_symbol2scm (glyph_name.c_str ());
  SCM entry = scm_hashq_ref (lily_character_table_, sym, SCM_BOOL_F);

  Offset o;
  if (entry == SCM_BOOL_F)
    return o;

  SCM char_alist = entry;
  SCM att_scm = scm_cdr (scm_assq (ly_symbol2scm ("attachment"), char_alist));

  return point_constant * ly_scm2offset (att_scm);
}

Box
Open_type_font::get_indexed_char (size_t signed_idx) const
{
  if (SCM_HASHTABLE_P (lily_index_to_bbox_table_))
    {
      SCM box = scm_hashq_ref (lily_index_to_bbox_table_,
			       scm_from_unsigned (signed_idx), SCM_BOOL_F);
      Box *box_ptr = Box::unsmob (box);
      if (box_ptr)
	return *box_ptr;
    }

  if (SCM_HASHTABLE_P (lily_character_table_))
    {
      const size_t len = 256;
      char name[len];
      size_t code = FT_Get_Glyph_Name (face_, signed_idx, name, len);
      if (code)
	warning (_f ("FT_Get_Glyph_Name() returned error: %u", unsigned (code)));

      SCM sym = ly_symbol2scm (name);
      SCM alist = scm_hashq_ref (lily_character_table_, sym, SCM_BOOL_F);

      if (alist != SCM_BOOL_F)
	{
	  SCM bbox = scm_cdr (scm_assq (ly_symbol2scm ("bbox"), alist));

	  Box b;
	  b[X_AXIS][LEFT] = scm_to_double (scm_car (bbox));
	  bbox = scm_cdr (bbox);
	  b[Y_AXIS][LEFT] = scm_to_double (scm_car (bbox));
	  bbox = scm_cdr (bbox);
	  b[X_AXIS][RIGHT] = scm_to_double (scm_car (bbox));
	  bbox = scm_cdr (bbox);
	  b[Y_AXIS][RIGHT] = scm_to_double (scm_car (bbox));
	  bbox = scm_cdr (bbox);

	  b.scale (point_constant);

	  scm_hashq_set_x (lily_index_to_bbox_table_,
			   scm_from_unsigned (signed_idx),
			   b.smobbed_copy ());
	  return b;
	}
    }

  FT_UInt idx = signed_idx;
  FT_Load_Glyph (face_,
		 idx,
		 FT_LOAD_NO_SCALE);

  FT_Glyph_Metrics m = face_->glyph->metrics;
  int hb = m.horiBearingX;
  int vb = m.horiBearingY;
  Box b (Interval (-hb, m.width - hb),
	 Interval (-vb, m.height - vb));

  b.scale (design_size () / Real (face_->units_per_EM));
  return b;
}

size_t
Open_type_font::name_to_index (string nm) const
{
  char *nm_str = (char *) nm.c_str ();
  if (size_t idx = FT_Get_Name_Index (face_, nm_str))
    return idx;
  
  return (size_t) -1;
}

size_t
Open_type_font::index_to_charcode (size_t i) const
{
  return ((Open_type_font *) this)->index_to_charcode_map_[i];
}

size_t
Open_type_font::count () const
{
  return ((Open_type_font *) this)->index_to_charcode_map_.size ();
}

Real
Open_type_font::design_size () const
{
  SCM entry = scm_hashq_ref (lily_global_table_,
			     ly_symbol2scm ("design_size"),

			     /*
			       Hmm. Design size is arbitrary for
			       non-design-size fonts. I vote for 1 -
			       which will trip errors more
			       quickly. --hwn.
			     */
			     scm_from_unsigned (1));
  return scm_to_double (entry) * Real (point_constant);
}

SCM
Open_type_font::sub_fonts () const
{
  return lily_subfonts_;
}

SCM
Open_type_font::get_char_table () const
{
  return lily_character_table_;
}

SCM
Open_type_font::get_subfonts () const
{
  return lily_subfonts_;
}

SCM
Open_type_font::get_global_table () const
{
  return lily_global_table_;
}

string
Open_type_font::font_name () const
{
  return FT_Get_Postscript_Name (face_);
}


SCM
Open_type_font::glyph_list () const
{
  SCM retval = SCM_EOL;
  SCM *tail = &retval;
  
  for (int i = 0; i < face_->num_glyphs; i++)
    {
      const size_t len = 256;
      char name[len];
      size_t code = FT_Get_Glyph_Name (face_, i, name, len);
      if (code)
	warning (_f ("FT_Get_Glyph_Name() returned error: %u", unsigned (code)));

      *tail = scm_cons (scm_from_locale_string (name), SCM_EOL);
      tail = SCM_CDRLOC (*tail);
    }
  
  return retval;
}