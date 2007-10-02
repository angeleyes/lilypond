/*
  staff-embosser.cc -- implement Staff_embosser

  source file of the GNU LilyPond music typesetter

  (c) 2007 Ralph Little
*/

#include "warn.hh"
#include "embosser-group.hh"
#include "context.hh"

class Staff_embosser : public Embosser
{
public:
  TRANSLATOR_DECLARATIONS (Staff_embosser);
  ~Staff_embosser ();

protected:
  virtual void finalize ();
  virtual void initialize ();
  void process_music ();
  void stop_translation_timestep ();

private:
};

#include "translator.icc"

ADD_TRANSLATOR (Staff_embosser, "", "",
		"", "");

Staff_embosser::Staff_embosser ()
{
}

Staff_embosser::~Staff_embosser ()
{
}

void
Staff_embosser::initialize ()
{
}

void
Staff_embosser::process_music ()
{
}

void
Staff_embosser::stop_translation_timestep ()
{
}

void
Staff_embosser::finalize ()
{
}

