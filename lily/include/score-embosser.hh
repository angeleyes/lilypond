/*
  score-embosser.hh -- declare Score_embosser

  (c) 2007 Ralph Little
*/

#ifndef SCORE_EMBOSSER_HH
#define SCORE_EMBOSSER_HH

#include "moment.hh"
#include "embosser-group.hh"

class Score_embosser : public Embosser_group
{
public:
  VIRTUAL_COPY_CONSTRUCTOR (Translator_group, Score_embosser);
  Embosser_output *performance_;

  ~Score_embosser ();
  Score_embosser ();

protected:
  DECLARE_LISTENER (finish);
  DECLARE_LISTENER (prepare);
  DECLARE_LISTENER (one_time_step);

  /* Engraver_group_engraver interface */
  virtual void connect_to_context (Context *);
  virtual void disconnect_from_context ();
  virtual void initialize ();
  virtual void derived_mark () const;

};

#endif // SCORE_EMBOSSER_HH
