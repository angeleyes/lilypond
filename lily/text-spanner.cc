/*

  text-spanner.cc -- implement Text_spanner

  source file of the GNU LilyPond music typesetter

  (c) 2000--2003 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include "molecule.hh"
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

/*
  TODO:
  - vertical start / vertical end (fixme-name) |
  - contination types (vert. star, vert. end)  |-> eat volta-bracket
  - more styles
  - more texts/positions
*/

MAKE_SCHEME_CALLBACK (Text_spanner, brew_molecule, 1);

/*
  TODO: this function is too long, too hairy.

  TODO: document this. What the heck is happening here?
*/
SCM
Text_spanner::brew_molecule (SCM smob) 
{
  Grob *me= unsmob_grob (smob);
  Spanner *spanner = dynamic_cast<Spanner*> (me);
  
  if (spanner->internal_has_interface (ly_symbol2scm ("piano-pedal-interface")))
    {
      setup_pedal_bracket(spanner);
    }

  /* Ugh, must be same as Hairpin::brew_molecule.  */
  Real padding = 0.0;
  SCM itp= me->get_grob_property ("if-text-padding");
  if (gh_number_p (itp))
    padding = gh_scm2double (itp);

  Grob *common = spanner->get_bound (LEFT)->common_refpoint (spanner->get_bound (RIGHT), X_AXIS);
  
  Interval span_points;
  Drul_array<bool> broken;
  Direction d = LEFT;
  do
    {
      Item *b = spanner->get_bound (d);
      broken[d] = b->break_status_dir () != CENTER;

      if (broken[d])
	{
	if (d == LEFT)
	  span_points[d] = spanner->get_broken_left_end_align ();
	else
	  span_points[d] = b->relative_coordinate (common, X_AXIS);
	}
      else
	  {
	    bool encl = to_boolean (me->get_grob_property ("enclose-bounds"));
	    span_points[d] = b->extent (common, X_AXIS)[encl ? d : -d];
	  }
    }
  while (flip (&d) != LEFT);


#if 0
  /*
    FIXME. - this switch   sucks. --hwn
   */
  // FIXME: ecs tells us -- only for (de)cresc. spanners
  width += gh_scm2double (me->get_grob_property ("width-correct"));
#endif

  SCM properties = Font_interface::font_alist_chain (me);
  SCM edge_text = me->get_grob_property ("edge-text");
  Drul_array<Molecule> edge;
  if (gh_pair_p (edge_text))
    {
      Direction d = LEFT;
      do
	{
	  /*  Don't repeat edge text for broken end */
	  if (broken[d])
	    continue;
	  
	  SCM text = index_get_cell (edge_text, d);
	  edge[d] = Text_item::text2molecule (me, text, properties);
	  if (!edge[d].empty_b ())
	    edge[d].align_to (Y_AXIS, CENTER);
	}
      while (flip (&d) != LEFT);
    }


  Drul_array<Real> shorten;
  shorten[LEFT] = 0;
  shorten[RIGHT] = 0;

  SCM s = me->get_grob_property ("shorten-pair");
  if (gh_pair_p (s))
    {
      span_points[LEFT] += gh_scm2double (ly_car (s));
      span_points[RIGHT] -= gh_scm2double (ly_cdr (s));
    }
  
  Real thick = me->get_paper ()->get_var ("linethickness");  
  SCM st = me->get_grob_property ("thickness");
  if (gh_number_p (st))
    {
      thick *=  gh_scm2double (st);
    }
  
  Drul_array<Molecule> edge_line;
  s = me->get_grob_property ("edge-height");
  SCM ew = me->get_grob_property ("edge-widen");
  if (gh_pair_p (s))
    {
      Direction d = LEFT;
      int dir = to_dir (me->get_grob_property ("direction"));
      do
	{
	  if (broken[d])
	    continue;
	  
	  Real dx = (gh_pair_p (ew)  ? 
		     gh_scm2double (index_get_cell (ew, d)) * d :  
		     0);
	  Real dy = gh_scm2double (index_get_cell (s, d)) * - dir;
	  if (dy)
	    {
	      edge_line[d] = Line_spanner::line_molecule (me, thick, dx, dy);
	    }
	}
      while (flip (&d) != LEFT);
    }
  
  Molecule m;
  do
    {
      Interval ext = edge[d].extent (X_AXIS);

      edge[d].translate_axis (span_points[d], X_AXIS);
      m.add_molecule (edge[d]);
      edge_line[d].translate_axis (span_points[d], X_AXIS);
      m.add_molecule (edge_line[d]);
      if (!ext.empty_b ())
	span_points[d] += -d *  ext[-d];
    }
  while (flip (&d) != LEFT);

  Molecule l =Line_spanner::line_molecule (me, thick,
					   span_points.length (), 0);
  l.translate_axis (span_points[LEFT], X_AXIS); 
  m.add_molecule (l);

  m.translate_axis (- me->relative_coordinate (common, X_AXIS), X_AXIS);
  return m.smobbed_copy ();
}




/* 
   Piano pedal brackets are a special case of a text spanner.
   Pedal up-down (restart) indicated by the angled right and left edges 
   of consecutive pedals touching exactly to form an __/\__
   Chris Jackson <chris@fluffhouse.org.uk>
*/

void 
Text_spanner::setup_pedal_bracket(Spanner *me)
{

  Real thick = me->get_paper ()->get_var ("linethickness");  
  SCM st = me->get_grob_property ("thickness");
  if (gh_number_p (st))
    {
      thick *=  gh_scm2double (st);
    }  

  Drul_array<bool> broken;
  Drul_array<Real> height, width, shorten, r;

  SCM pa = me->get_grob_property ("if-text-padding");
  SCM ew = me->get_grob_property ("edge-widen");
  SCM eh = me->get_grob_property ("edge-height");
  SCM sp = me->get_grob_property ("shorten-pair");
  
  Direction d = LEFT;
  Interval e;
  Real padding = 0;

  if (gh_number_p (pa) )
    padding = gh_scm2double (pa);

  do
    {
      Item *b = me->get_bound (d);

      e = b->extent (b, X_AXIS);
      if (!e.empty_b ())
	r[d] = d * (e[-d] + padding);

      broken[d] = b->break_status_dir () != CENTER;
      width[d]  =  0;
      height[d] =  0;
      shorten[d] = 0;
      if ( ly_number_pair_p (ew) )
	width[d] +=  gh_scm2double (index_get_cell (ew, d));
      if ( !broken[d] && (ly_number_pair_p (eh) ) )
	height[d] += gh_scm2double (index_get_cell (eh, d));
      if ( ly_number_pair_p (sp) )
	shorten[d] +=  gh_scm2double (index_get_cell (sp, d));
    }
  while (flip (&d) != LEFT);
  
  Real extra_short = 0;
  // For 'Mixed' style pedals, i.e.  a bracket preceded by text:  Ped._____|
  // need to shorten by the extent of the text grob
  if ( to_boolean (me->get_grob_property ("text-start")) )
    {
      height[LEFT] = 0;
      extra_short = padding;
      if (Grob *textbit = unsmob_grob (me->get_grob_property("pedal-text")))
	{
	  if (textbit->internal_has_interface(ly_symbol2scm("text-interface")))
	    // for plain text, e.g., Sost. Ped.
	    {
	      SCM text  =  textbit->get_grob_property("text"); 
	      if (gh_string_p (text)) {
		SCM properties = Font_interface::font_alist_chain (me);
		Molecule mol = Text_item::text2molecule (me, text, properties);
		extra_short += mol.extent(X_AXIS).length() / 2;
	      }
	    }
	}
      shorten[RIGHT] -= thick;
    }

  shorten[LEFT] += extra_short ;
  
  if (broken[LEFT])
    {
      shorten[LEFT]  -=  me->get_broken_left_end_align () ;
      shorten[RIGHT]  +=  abs(width[RIGHT])  +  thick  -  r[RIGHT];
    }

  else
    {
      // Shorten a ____/ on the right so that it will touch an adjoining \___
      shorten[RIGHT]  +=  abs(width[LEFT])  +  abs(width[RIGHT])  +  thick;
      // Also shorten so that it ends just before the spanned note.
      shorten[RIGHT]  -=  (r[LEFT]  +  r[RIGHT]);
    }

  me->set_grob_property ("edge-height", ly_interval2scm (height));
  me->set_grob_property ("edge-widen",  ly_interval2scm(width));
  me->set_grob_property ("shorten-pair", ly_interval2scm (shorten));
}


struct Pianopedal
{
  static bool has_interface (Grob*);
};
ADD_INTERFACE (Pianopedal,"piano-pedal-interface",
	       "",
	       "pedal-type edge-widen edge-height shorten-pair text-start left-widen right-widen pedal-text");

ADD_INTERFACE (Text_spanner,"text-spanner-interface",
	       "generic text spanner",
	       "dash-period if-text-padding dash-length edge-height edge-widen edge-text shorten-pair type thickness enclose-bounds width-correct");

