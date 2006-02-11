/*
  modified-font-metric.cc -- declare Modified_font_metric

  source file of the GNU LilyPond music typesetter

  (c) 1999--2006 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/
#include <cctype>
using namespace std;

#include "modified-font-metric.hh"
#include "pango-font.hh"
#include "text-metrics.hh"
#include "warn.hh"
#include "stencil.hh"
#include "lookup.hh"
#include "main.hh"

Modified_font_metric::Modified_font_metric (Font_metric *fm,
					    Real magnification)
{
  magnification_ = magnification;

  SCM desc = fm->description_;

  Real total_mag = magnification * scm_to_double (scm_cdr (desc));
  assert (total_mag);

  description_ = scm_cons (scm_car (desc), scm_from_double (total_mag));
  orig_ = fm;
}

SCM
Modified_font_metric::make_scaled_font_metric (Font_metric *fm, Real scaling)
{
  Modified_font_metric *sfm = new Modified_font_metric (fm, scaling);
  return sfm->self_scm ();
}

Real
Modified_font_metric::design_size () const
{
  return orig_->design_size ();
}

Box
Modified_font_metric::get_indexed_char (vsize i) const
{
  Box b = orig_->get_indexed_char (i);
  b.scale (magnification_);
  return b;
}

Box
Modified_font_metric::get_ascii_char (vsize i) const
{
  Box b = orig_->get_ascii_char (i);
  b.scale (magnification_);
  return b;
}

vsize
Modified_font_metric::count () const
{
  return orig_->count ();
}

Offset
Modified_font_metric::attachment_point (string s) const
{
  Offset o = orig_->attachment_point (s);
  return o * magnification_;
}

Offset
Modified_font_metric::get_indexed_wxwy (vsize k) const
{
  Offset o = orig_->get_indexed_wxwy (k);
  return o * magnification_;
}

vsize
Modified_font_metric::name_to_index (string s) const
{
  return orig_->name_to_index (s);
}

vsize
Modified_font_metric::index_to_charcode (vsize i) const
{
  return orig_->index_to_charcode (i);
}

vsize
Modified_font_metric::index_to_ascii (vsize k) const
{
  return orig_->index_to_ascii (k);
}

void
Modified_font_metric::derived_mark () const
{
}

/* TODO: put this klutchness behind ly:option switch.  */
Box
Modified_font_metric::tex_kludge (string text) const
{
  Interval ydims;
  Real w = 0;
  for (ssize i = 0; i < text.length (); i++)
    {
      switch (text[i])
	{
	case '\\':
	  /* Accent marks use width of base letter */
	  if (i < text.length () - 1)
	    {
	      if (text[i + 1]=='\'' || text[i + 1]=='`' || text[i + 1]=='"'
		  || text[i + 1]=='^')
		{
		  i++;
		  break;
		}
	      /* For string width \\ is a \ and \_ is a _. */
	      if (text[i + 1]=='\\' || text[i + 1]=='_')
		break;
	    }

	  for (i++; (i < text.length ()) && !isspace (text[i])
		 && text[i]!='{' && text[i]!='}'; i++)
	    ;

	  /* Compensate for the auto-increment in the outer loop. */
	  i--;
	  break;

	case '{': // Skip '{' and '}'
	case '}':
	  break;

	default:
	  Box b = get_ascii_char ((unsigned char)text[i]);

	  /* Use the width of 'x' for unknown characters */
	  if (b[X_AXIS].length () == 0)
	    b = get_ascii_char ((unsigned char)'x');

	  w += b[X_AXIS].length ();
	  ydims.unite (b[Y_AXIS]);
	  break;
	}
    }

  if (ydims.is_empty ())
    ydims = Interval (0, 0);

  return Box (Interval (0, w), ydims);
}

Stencil
Modified_font_metric::text_stencil (string text) const
{
  Box b;
  if (Pango_font *pf = dynamic_cast<Pango_font *> (orig_))
    {
      Stencil stc = pf->text_stencil (text);

      Box b = stc.extent_box ();

      b.scale (magnification_);
      Stencil scaled (b, stc.expr ());
      return scaled;
    }

  return Font_metric::text_stencil (text);
}

Box
Modified_font_metric::text_dimension (string text) const
{
  SCM stext = scm_makfrom0str (text.c_str ());
  Box b = lookup_tex_text_dimension (orig_, stext);
  if (!b[Y_AXIS].is_empty ())
    {
      b.scale (magnification_);
      return b;
    }

  if (output_backend_global == "tex")
    {
      b = tex_kludge (text);
      return b;
    }

  Interval ydims;

  Real w = 0.0;

  for (ssize i = 0; i < text.length (); i++)
    {
      Box b = get_ascii_char ((unsigned char)text[i]);

      w += b[X_AXIS].length ();
      ydims.unite (b[Y_AXIS]);
    }
  if (ydims.is_empty ())
    ydims = Interval (0, 0);

  b = Box (Interval (0, w), ydims);
  return b;
}

Font_metric *
Modified_font_metric::original_font () const
{
  return orig_;
}

SCM
Modified_font_metric::sub_fonts () const
{
  return orig_->sub_fonts ();
}

string
Modified_font_metric::font_name () const
{
  return original_font ()->font_name ();
}
