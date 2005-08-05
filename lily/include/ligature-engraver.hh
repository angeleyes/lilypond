/*
  ligature-engraver.hh -- declare Ligature_engraver

  source file of the GNU LilyPond music typesetter

  (c) 2002--2005 Juergen Reuter <reuter@ipd.uka.de>
*/
#ifndef LIGATURE_ENGRAVER_HH
#define LIGATURE_ENGRAVER_HH

#include "engraver.hh"
#include "moment.hh"

class Ligature_engraver : public Engraver
{
protected:
  Ligature_engraver ();
  void stop_translation_timestep ();
  virtual void finalize ();


  DECLARE_ACKNOWLEDGER (rest);
  DECLARE_ACKNOWLEDGER (note_head);
  virtual bool try_music (Music *);
  void process_music ();
  virtual Spanner *create_ligature_spanner () = 0;
  virtual void typeset_ligature (Spanner *ligature,
				 Array<Grob_info> primitives) = 0;
  virtual Spanner *current_ligature ();
  SCM brew_ligature_primitive_proc;

public:
  // no TRANSLATOR_DECLARATIONS (Ligature_engraver) needed since this
  // class is abstract

private:
  Drul_array<Music *> events_drul_;

  Spanner *ligature_;
  Array<Grob_info> primitives_;

  Spanner *finished_ligature_;
  Array<Grob_info> finished_primitives_;

  Music *prev_start_event_;

  // moment where ligature started.
  Moment ligature_start_mom_;

  Grob *last_bound_;

  void override_stencil_callback ();
  void revert_stencil_callback ();
};

#endif // LIGATURE_ENGRAVER_HH
