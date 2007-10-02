/*
  embosser-output.cc -- implement Embosser_output - produce Braille output

  source file of the GNU LilyPond music typesetter

  (c) 2007 Ralph Little
*/

#include "embosser-output.hh"

#include <ctime>
using namespace std;

#include "file-name.hh"
#include "international.hh"
#include "lily-version.hh"
#include "main.hh"
#include "score.hh"
#include "string-convert.hh"
#include "warn.hh"

Embosser_output::Embosser_output ()
{
}

Embosser_output::~Embosser_output ()
{
}

void
Embosser_output::output () const
{
}

void
Embosser_output::write_output (string out) const
{
}

void
Embosser_output::process ()
{
}

Embosser_output *
unsmob_embosser_output (SCM x)
{
  return dynamic_cast<Embosser_output*> (unsmob_music_output (x));
}
