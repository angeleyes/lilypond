/*   
  text-item.cc -- implement Text_interface

  source file of the GNU LilyPond music typesetter
  
  (c) 1998--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
                 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include "text-item.hh"

#include <math.h>

#include "main.hh" 
#include "config.hh"
#include "pango-font.hh"
#include "warn.hh"
#include "grob.hh"
#include "font-interface.hh"
#include "output-def.hh"
#include "modified-font-metric.hh"


MAKE_SCHEME_CALLBACK (Text_interface, interpret_string, 3)
SCM
Text_interface::interpret_string (SCM layout_smob,
				  SCM props,
				  SCM markup)
{
  Output_def *layout = unsmob_output_def (layout_smob);
  
  SCM_ASSERT_TYPE (layout, layout_smob, SCM_ARG1,
		   __FUNCTION__, "Layout definition");
  SCM_ASSERT_TYPE (scm_is_string (markup), markup, SCM_ARG3,
		   __FUNCTION__, "string");

  String str = ly_scm2string (markup);

  Font_metric *fm = select_encoded_font (layout, props);
  return fm->text_stencil (str).smobbed_copy ();
}


MAKE_SCHEME_CALLBACK (Text_interface, interpret_markup, 3)
SCM
Text_interface::interpret_markup (SCM layout_smob, SCM props, SCM markup)
{
  if (scm_is_string (markup))
    return interpret_string (layout_smob, props, markup);
  else if (scm_is_pair (markup))
    {
      SCM func = scm_car (markup);
      SCM args = scm_cdr (markup);
      if (!markup_p (markup))
	programming_error ("Markup head has no markup signature.");
      
      return scm_apply_2 (func, layout_smob, props, args);
    }
  else
    {
      programming_error ("Is not a markup: ");
      scm_display (markup, scm_current_error_port());
      assert (false);
      Box b;
      b[X_AXIS].set_empty();
      b[Y_AXIS].set_empty();
      
      Stencil s(b, SCM_EOL);
      return s.smobbed_copy();
    }
}

MAKE_SCHEME_CALLBACK (Text_interface, print, 1);
SCM
Text_interface::print (SCM grob)
{
  Grob *me = unsmob_grob (grob);
  
  SCM t = me->get_property ("text");
  SCM chain = Font_interface::text_font_alist_chain (me);
  return interpret_markup (me->get_layout ()->self_scm (), chain, t);
}

/* Ugh. Duplicated from Scheme.  */
bool
Text_interface::markup_p (SCM x)
{
  return (scm_is_string (x)
	  || (scm_is_pair (x)
	      && SCM_BOOL_F
	      != scm_object_property (scm_car (x),
				      ly_symbol2scm ("markup-signature"))));
}

ADD_INTERFACE (Text_interface, "text-interface",
	       "A scheme markup text, see @usermanref{Text-markup}.",
	       "text baseline-skip word-space");




