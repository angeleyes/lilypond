/*
  embosser-output.hh -- declare Embosser_output

  (c) 2007 Ralph Little
*/

#ifndef EMBOSSER_OUTPUT_HH
#define EMBOSSER_OUTPUT_HH

#include "std-vector.hh"
#include "music-output.hh"

class Embosser_output : public Music_output
{
public:
  Embosser_output ();
  ~Embosser_output ();
  DECLARE_CLASSNAME(Embosser_output);

  virtual void process ();
  void output () const;
  void print () const;
  void write_output (string filename) const;
};

#endif /* EMBOSSER_OUTPUT_HH */
