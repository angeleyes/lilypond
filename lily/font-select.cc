/*
  font-select.cc -- implement property -> font_metric routines.

  source file of the GNU LilyPond music typesetter

  (c) 2003--2005 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include <math.h>

#include "dimensions.hh"
#include "all-font-metrics.hh"
#include "output-def.hh"
#include "font-interface.hh"
#include "warn.hh"
#include "pango-font.hh"
#include "main.hh"

Font_metric *
get_font_by_design_size (Output_def *layout, Real requested,
			 SCM font_vector)
{
  int n = scm_c_vector_length (font_vector);
  Real size = 1e6;
  Real last_size = -1e6;
  int i = 0;

  SCM pango_description_string = SCM_EOL;
  SCM last_pango_description_string = SCM_EOL;
  for (; i < n; i++)
    {
      SCM entry = scm_c_vector_ref (font_vector, i);

      if (scm_promise_p (entry) == SCM_BOOL_T)
	{
	  Font_metric *fm = unsmob_metrics (scm_force (entry));
	  size = fm->design_size ();
	}
#if HAVE_PANGO_FT2
      else if (scm_is_pair (entry)
	       && scm_is_number (scm_car (entry))
	       && scm_is_string (scm_cdr (entry)))
	{
	  size = scm_to_double (scm_car (entry));
	  pango_description_string
	    = scm_cdr (entry);
	}
#endif

      if (size > requested)
	break;
      last_size = size;
      last_pango_description_string = pango_description_string;
    }

  if (i == n)
    i = n - 1;
  else if (i > 0)
    {
      if ((requested / last_size) < (size / requested))
	{
	  i--;
	  size = last_size;
	  pango_description_string = last_pango_description_string;
	}
    }

  Font_metric *fm = 0;
  if (scm_is_string (pango_description_string))
    {
#if HAVE_PANGO_FT2
      return find_pango_font (layout,
			      pango_description_string,
			      requested / size);
#else
      error ("Trying to retrieve pango font without HAVE_PANGO_FT2.");
#endif
    }
  else
    {
      fm = unsmob_metrics (scm_force (scm_c_vector_ref (font_vector, i)));
    }

  return find_scaled_font (layout, fm, requested / size);
}

Font_metric *
get_font_by_mag_step (Output_def *layout, Real requested_step,
		      SCM font_vector, Real default_size)
{
  return get_font_by_design_size (layout, default_size
				  * pow (2.0, requested_step / 6.0),
				  font_vector);
}

SCM
properties_to_font_size_family (SCM fonts, SCM alist_chain)
{
  return scm_call_2 (ly_lily_module_constant ("lookup-font"), fonts,
		     alist_chain);
}

Font_metric *
select_encoded_font (Output_def *layout, SCM chain)
{
  SCM name = ly_chain_assoc_get (ly_symbol2scm ("font-name"), chain, SCM_BOOL_F);

  if (!scm_is_string (name))
    {
      SCM fonts = layout->lookup_variable (ly_symbol2scm ("fonts"));
      name = properties_to_font_size_family (fonts, chain);
    }

#if HAVE_PANGO_FT2
  if (scm_is_string (name)
      && is_pango_format_global)
    {
      return select_pango_font (layout, chain);
    }
  else
#endif
    if (scm_is_string (name))
      {
	Real rmag
	  = robust_scm2double (ly_chain_assoc_get (ly_symbol2scm ("font-magnification"), chain, SCM_BOOL_F),
			       1.0);
	Font_metric *fm = all_fonts_global->find_font (ly_scm2string (name));

	return find_scaled_font (layout, fm, rmag);
      }
    else if (scm_instance_p (name))
      {
	SCM base_size = scm_slot_ref (name, ly_symbol2scm ("default-size"));
	SCM vec = scm_slot_ref (name, ly_symbol2scm ("size-vector"));

	Real req = robust_scm2double (ly_chain_assoc_get (ly_symbol2scm ("font-size"), chain, SCM_BOOL_F),
				      0.0);

	return get_font_by_mag_step (layout, req, vec,
				     scm_to_double (base_size));
      }

  assert (0);
  return 0;
}

Font_metric *
select_font (Output_def *layout, SCM chain)
{
  return select_encoded_font (layout, chain);
}
