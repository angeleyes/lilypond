/*
  lyric-extender.cc -- implement Lyric_extender
  source file of the GNU LilyPond music typesetter

  (c)  1998--2003 Jan Nieuwenhuizen <janneke@gnu.org>
  Han-Wen Nienhuys
*/


#include "box.hh"
#include "warn.hh"
#include "lookup.hh"
#include "molecule.hh"
#include "paper-column.hh"
#include "paper-def.hh"
#include "lyric-extender.hh"


MAKE_SCHEME_CALLBACK (Lyric_extender,brew_molecule,1)
SCM 
Lyric_extender::brew_molecule (SCM smob) 
{
  Spanner *sp = unsmob_spanner (smob);
  Item* l = sp->get_bound (LEFT);
  Item*r = sp->get_bound (RIGHT);
  
  Real leftext = l->extent (l, X_AXIS).length ();

  Real sl = sp->get_paper ()->get_var ("linethickness");  
  Real righttrim = 0.5; // default to half a space gap on the right


  /*
    If we're broken, we shouldn't extend past the end of the line.
   */
  if (r->break_status_dir () == CENTER)
    {
      SCM righttrim_scm = sp->get_grob_property ("right-trim-amount");
      if (gh_number_p (righttrim_scm))
	{
	  righttrim = gh_scm2double (righttrim_scm);
	}
    }
  
  // The extender can exist in the word space of the left lyric ...
  SCM space =  sp->get_bound (LEFT)->get_grob_property ("word-space");
  if (gh_number_p (space))
    {
      leftext -=  gh_scm2double (space);
    }
  Real w = sp->spanner_length () - leftext - righttrim;
  
  Real h = sl * gh_scm2double (sp->get_grob_property ("height"));
  Molecule  mol (Lookup::filledbox (Box (Interval (0,w), Interval (0,h))));
  mol.translate (Offset (leftext, 0));
  return mol.smobbed_copy ();
}

void
Lyric_extender::set_textitem (Spanner*sp, Direction d, Grob*s)
{
  sp->set_bound (d, s);
  sp->add_dependency (s);
}




ADD_INTERFACE (Lyric_extender,"lyric-extender-interface",
  "The extender is a simple line at the baseline of the lyric "
" that helps show the length of a melissima (tied/slurred note).",
  "word-space height right-trim-amount");
