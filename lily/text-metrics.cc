/*
  text-metrics.cc -- implement text metric lookup functions

  source file of the GNU LilyPond music typesetter

  (c) 2004 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "dimensions.hh"
#include "font-metric.hh"
#include "main.hh"

static SCM text_dimension_hash_tab;


Box
lookup_tex_text_dimension (Font_metric *font,
			   SCM text)
{
  Box b;

  SCM limit = ly_lily_module_constant ("TEX_STRING_HASHLIMIT");
  String key_str = ly_scm2string (font->font_file_name());
  int hash_code = scm_to_int (scm_hash (text, limit));
  key_str = to_string (hash_code)  + key_str;
  
  SCM val = SCM_BOOL_F;
  if (text_dimension_hash_tab)
    {
      scm_hash_ref (text_dimension_hash_tab,
		    scm_makfrom0str (key_str.to_str0 ()),
		    SCM_BOOL_F);
    }
  if (scm_is_pair (val))
    {
      b[X_AXIS][LEFT] = 0.0;
      b[X_AXIS][RIGHT] = scm_to_double (scm_car (val)) * point_constant;
      val = scm_cdr (val);
      b[Y_AXIS][UP] = scm_to_double (scm_car (val)) * point_constant;
      val = scm_cdr (val);
      b[Y_AXIS][DOWN] = scm_to_double (scm_car (val)) * point_constant; 
    }
  
  return b; 
}



LY_DEFINE(ly_load_text_dimensions, "ly:load-text-dimensions",
	  1, 0, 0,
	  (SCM dimension_alist),
	  "Load dimensions from TeX in a (KEY . (W H D)) format alist")
{
  if (!text_dimension_hash_tab)
    {
      text_dimension_hash_tab =
	scm_gc_protect_object (scm_make_hash_table (scm_from_int (113)));
    }

  for (SCM s = dimension_alist;
       scm_is_pair (s);
       s = scm_cdr (s))
    {
      SCM key = scm_caar (s);
      SCM val = scm_cdar (s);
      
      if (scm_hash_ref (text_dimension_hash_tab, key, SCM_BOOL_F)
	  == SCM_BOOL_F)
	{
	  scm_hash_set_x (text_dimension_hash_tab, key, val);
	}
    }

  return SCM_UNSPECIFIED;
}

void
try_load_text_metrics (String basename)
{
  String path =  global_path.find_file (basename +  ".textmetrics");
  if (path != "")
    {
      String contents (gulp_file_to_string (path));
      contents = "(quote (" +  contents + "))";

      SCM lst = scm_c_eval_string (contents.to_str0 ());
      ly_load_text_dimensions (lst);
    }
}
