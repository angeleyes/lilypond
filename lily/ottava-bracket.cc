/*   
  ottava-bracket.cc --  implement Ottava_bracket

  source file of the GNU LilyPond music typesetter

  (c) 2004 Han-Wen Nienhuys <hanwen@xs4all.nl>

 */

#include "stencil.hh"
#include "text-item.hh"
#include "text-spanner.hh"
#include "line-spanner.hh"
#include "spanner.hh"
#include "font-interface.hh"
#include "dimensions.hh"
#include "paper-def.hh"
#include "warn.hh"
#include "paper-column.hh"
#include "staff-symbol-referencer.hh"
#include "note-column.hh"
#include "directional-element-interface.hh"
#include "tuplet-bracket.hh"

struct Ottava_bracket
{
  DECLARE_SCHEME_CALLBACK (print, (SCM));
  static bool has_interface (Grob*);
};


/*
  TODO: the string for ottava shoudl depend on the available space, ie.

  
  Long: 15ma        Short: 15ma    Empty: 15
         8va                8va            8
         8va bassa          8ba            8

*/

MAKE_SCHEME_CALLBACK (Ottava_bracket, print, 1);
SCM
Ottava_bracket::print (SCM smob)
{
  Spanner*me  = dynamic_cast<Spanner*> (unsmob_grob (smob));
  
  
  Interval span_points;
  
  Grob *common = me->get_bound (LEFT)->common_refpoint (me->get_bound (RIGHT), X_AXIS);
  Paper_def * paper = me->get_paper();

  
  Drul_array<bool> broken;
  Direction d = LEFT;
  do
    {
      Item *b = me->get_bound (d);
      broken[d] = (b->break_status_dir () != CENTER);

      if (Note_column::has_interface (b))
	{
	  common = common_refpoint_of_list (b->get_grob_property ("heads"), common, X_AXIS);
	}
    }
  while (flip (&d) != LEFT);

  SCM properties = Font_interface::font_alist_chain (me);
  SCM markup = me->get_grob_property ("text");
  Stencil text;
  if (Text_item::markup_p (markup)) 
    text = *unsmob_stencil (Text_item::interpret_markup (paper->self_scm (), properties, markup));


  Drul_array<Real> shorten = robust_scm2interval (me->get_grob_property ("shorten-pair"),
						  Interval (0,0));


  /*
    TODO: we should check if there are ledgers, and modify length of
    the spanner to that.
   */
  do
    {
      Item *b = me->get_bound (d);

      Interval ext;
      if (Note_column::has_interface (b))
	{
	  for (SCM s = b->get_grob_property ("note-heads"); gh_pair_p (s); s =gh_cdr (s))
	    ext.unite (unsmob_grob (gh_car (s))->extent (common, X_AXIS));
	}

      if (ext.is_empty ())
	ext = Interval (0,0);
      
      span_points[d] =  (broken [d]) ? b->extent (common, X_AXIS)[-d] : ext[d];

      if (broken[d])
	shorten [d] = 0.0; 
    }
  while (flip (&d) != LEFT);


  /*
    0.3 is ~ italic correction. 
   */
  Real text_size =  text.extent (X_AXIS).is_empty ()
    ? 0.0 : text.extent (X_AXIS)[RIGHT] +  0.3;
  
  span_points[LEFT] = span_points[LEFT]
    <? (span_points[RIGHT] - text_size
	- robust_scm2double (me->get_grob_property ("minimum-length"), -1.0)); 
  
  Interval bracket_span_points = span_points;
  bracket_span_points[LEFT] += text_size;
  
  Drul_array<Real> edge_height = robust_scm2interval (me->get_grob_property ("edge-height"),
						      Interval (1.0, 1.0));

  
  Drul_array<Real> flare = robust_scm2interval (me->get_grob_property ("bracket-flare"),
						Interval (0,0));



  edge_height[LEFT] = 0.0;
  edge_height[RIGHT] *=  - get_grob_direction (me);
  if (broken[RIGHT])
    edge_height[RIGHT] = 0.0;
  
  Stencil b;
  if (!bracket_span_points.is_empty () && bracket_span_points.length () > 0.001)
    b = Tuplet_bracket::make_bracket (me,
				      Y_AXIS, Offset (bracket_span_points.length (), 0),
				       edge_height,
				      0.0,
				      flare, shorten);

  /*
    The vertical lines should not take space, for the following scenario:

    8 -----+
        o  |
       |
       |
       

    Just a small amount, yes.  In tight situations, it is even
    possible to center the `8' directly below the note, dropping the
    ottava line completely...

  */
  
  b = Stencil (Box (b.extent (X_AXIS),
		     Interval (0.1,0.1)),
		b.get_expr ());
  
  b.translate_axis (bracket_span_points[LEFT], X_AXIS);
  text.translate_axis (span_points[LEFT], X_AXIS);
  text.align_to (Y_AXIS, CENTER);
  b.add_stencil (text);
  
  b.translate_axis (- me->relative_coordinate (common, X_AXIS), X_AXIS);
  
  return b.smobbed_copy ();  
}


ADD_INTERFACE (Ottava_bracket, "ottava-bracket-interface",
	       "An ottava bracket",
	       "edge-height bracket-flare shorten-pair minimum-length");

