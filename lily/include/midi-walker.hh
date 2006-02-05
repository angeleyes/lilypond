/*
  midi-walker.hh -- declare Midi_walker

  (c) 1996--2006 Han-Wen Nienhuys  <hanwen@xs4all.nl>
  Jan Nieuwenhuizen <janneke@gnu.org>
*/

#ifndef MIDI_WALKER_HH
#define MIDI_WALKER_HH

#include "pqueue.hh"
#include "lily-proto.hh"
#include "moment.hh"

struct Midi_note_event : PQueue_ent<Moment, Midi_note *>
{
  bool ignore_b_;
  Midi_note_event ();
};

int compare (Midi_note_event const &left, Midi_note_event const &right);

/**
   walk audio and output midi
*/
class Midi_walker
{
public:
  Midi_walker (Audio_staff *audio_staff, Midi_track *midi_track);
  ~Midi_walker ();

  void process ();
  void operator ++ (int);
  bool ok () const;

private:
  void do_start_note (Midi_note *note);
  void do_stop_notes (Moment now_mom);
  void output_event (Moment now_mom, Midi_item *l);

  Midi_track *track_;
  Audio_staff *staff_;
  vsize index_;
  Link_array__Audio_item_ *items_;
  PQueue<Midi_note_event> stop_note_queue;
  Moment last_mom_;
};

#endif // MIDI_WALKER_HH
