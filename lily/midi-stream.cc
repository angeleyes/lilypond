//
// midistream.cc
//
// source file of the GNU LilyPond music typesetter
//
// (c) 1997 Jan Nieuwenhuizen <jan@digicash.com>

#include <fstream.h>
#include <time.h>
#include "string.hh"
#include "string-convert.hh"
#include "main.hh"
#include "misc.hh"
#include "midi-item.hh"
#include "midi-stream.hh"
#include "debug.hh"

Midi_stream::Midi_stream (String filename_str)
{
  filename_str_ = filename_str;
  os_p_ = 0;
  open();
}

Midi_stream::~Midi_stream()
{
  *os_p_ << flush;		// ugh. Share with tex_stream.
  if (!*os_p_)
    {
      warning(_("error syncing file (disk full?)"));
      exit_status_i_ = 1;
    }
  delete os_p_;
}

Midi_stream&
Midi_stream::operator <<(String str)
{
  if (check_debug && !monitor->silence("Midistrings"))
    str = String_convert::bin2hex_str (str);

  *os_p_ << str;

  if (check_debug && !monitor->silence("Midistrings"))
    *os_p_ << "\n";

  return *this;
}

Midi_stream&
Midi_stream::operator <<(Midi_item const& mitem_c_r)
{
  //    *this << mitem_c_r.str();
  mitem_c_r.output (this);
  if (check_debug && !monitor->silence("Midistrings"))
    *os_p_ << "\n";
  return *this;
}

Midi_stream&
Midi_stream::operator <<(int i)
{
  // output binary string ourselves
  *this << Midi_item::i2varint_str (i);
  return *this;
}

void
Midi_stream::open()
{
  os_p_ = new ofstream (filename_str_.ch_C ());
  if (!*os_p_)
    error (_("can't open `") + filename_str_ + "\'");
}
