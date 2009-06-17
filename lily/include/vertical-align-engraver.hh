/*
  vertical-align-engraver.hh -- declare Vertical_align_engraver

  source file of the GNU LilyPond music typesetter

  (c) 1997--2009 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#ifndef VERTICAL_ALIGN_ENGRAVER_HH
#define VERTICAL_ALIGN_ENGRAVER_HH

#include "engraver.hh"
#include "spanner.hh"

#include "translator.icc"

class Vertical_align_engraver : public Engraver
{
  SCM id_to_group_hashtab_;

public:
  TRANSLATOR_DECLARATIONS (Vertical_align_engraver);
  DECLARE_ACKNOWLEDGER (axis_group);

protected:
  Spanner *valign_;
  virtual bool qualifies (Grob_info) const;
  virtual void make_alignment ();
  virtual void add_element (Grob *elt);
  
  virtual void derived_mark () const;
  void process_music ();
  virtual void finalize ();
  virtual void initialize ();
};

#endif /* VERTICAL_ALIGN_ENGRAVER_HH */
