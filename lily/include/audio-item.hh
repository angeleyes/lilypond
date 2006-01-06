/*
  audio-item.hh -- declare Audio_items

  (c) 1996--2006 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#ifndef AUDIO_ITEM_HH
#define AUDIO_ITEM_HH

#include "audio-element.hh"
#include "moment.hh"
#include "pitch.hh"

/**

Any piece of audio information.  We need virtual constructors, let's
try decentralised factory for specific audio implemenations.
*/
class Audio_item : public Audio_element
{
public:
  Audio_item ();
  Audio_column *audio_column_;

private:
  Audio_item (Audio_item const &);
  Audio_item &operator = (Audio_item const &);
};

class Audio_dynamic : public Audio_item
{
public:
  Audio_dynamic (Real volume);

  Real volume_;
};

class Audio_key : public Audio_item
{
public:
  Audio_key (int acc, bool major);

  int accidentals_;
  bool major_;
};

class Audio_instrument : public Audio_item
{
public:
  Audio_instrument (String instrument_string);

  String str_;
};

class Audio_note : public Audio_item
{
public:
  Audio_note (Pitch p, Moment m, int transposing_i = 0);

  void tie_to (Audio_note *);

  Pitch pitch_;
  Moment length_mom_;
  int transposing_;
  Audio_note *tied_;
};

class Audio_piano_pedal : public Audio_item
{
public:
  String type_string_;
  Direction dir_;
};

class Audio_text : public Audio_item
{
public:
  enum Type
    {
      TEXT = 1, COPYRIGHT, TRACK_NAME, INSTRUMENT_NAME, LYRIC,
      MARKER, CUE_POINT
    };

  Audio_text (Audio_text::Type type, String text_string);

  Type type_;
  String text_string_;
};

class Audio_tempo : public Audio_item
{
public:
  Audio_tempo (int per_minute_4_i);

  int per_minute_4_;
};

class Audio_time_signature : public Audio_item
{
public:
  Audio_time_signature (int beats, int one_beat);

  int beats_;
  int one_beat_;
};

#endif // AUDIO_ITEM_HH

