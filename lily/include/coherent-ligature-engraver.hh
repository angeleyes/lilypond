/*
  coherent-ligature-engraver.hh -- declare Coherent_ligature_engraver

  source file of the GNU LilyPond music typesetter

  (c) 2003--2006 Juergen Reuter <reuter@ipd.uka.de>
*/
#ifndef COHERENT_LIGATURE_ENGRAVER_HH
#define COHERENT_LIGATURE_ENGRAVER_HH

#include "ligature-engraver.hh"

class Coherent_ligature_engraver : public Ligature_engraver
{
public:
  // no TRANSLATOR_DECLARATIONS (Coherent_ligature_engraver) needed
  // since this class is abstract

protected:
  virtual void build_ligature (Spanner *ligature,
			       std::vector<Grob_info> primitives) = 0;
  virtual void typeset_ligature (Spanner *ligature,
				 std::vector<Grob_info> primitives);
  virtual void get_set_column (Item *, Paper_column *);
private:
  void collect_accidentals (Spanner *, std::vector<Grob_info>);
};

#endif // COHERENT_LIGATURE_ENGRAVER_HH
