/*
  audio-column.hh -- declare Audio_column

  (c) 1997--2006 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#ifndef AUDIO_COLUMN_HH
#define AUDIO_COLUMN_HH

#include "lily-proto.hh"
#include "moment.hh"
#include "std-vector.hh"
#include "audio-element.hh"

/**
   generic audio grouped vertically.
*/

class Audio_column : public Audio_element
{
public:
  Audio_column (Moment when);

  void add_audio_item (Audio_item *i);
  Moment when () const;

  vector<Audio_item*> audio_items_;

protected:
  void offset_when (Moment m);
  friend class Score_performer;

private:
  Audio_column (Audio_column const &);

  Moment when_;
};

#endif // AUDIO_COLUMN_HH
