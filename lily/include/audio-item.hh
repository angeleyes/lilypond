/*
  audio-item.hh -- declare Audio_items

  (c) 1996, 1997 Jan Nieuwenhuizen <jan@digicash.com>
 */

#ifndef AUDIO_ITEM_HH
#define AUDIO_ITEM_HH

#include "lily-proto.hh"
#include "string.hh"
#include "audio-element.hh"

/**
  Any piece of audio information.
  We need virtual constructors, 
  let's try decentralised factory for specific audio implemenations.

  TODO:

  virtual Niff_item* niff_item_p() = 0;
  
  virtual CSound_item* score_item_p() = 0;
 */
struct Audio_item : public Audio_element {
  Audio_item (Request* req_l);

  /// Create a midi-item from myself.
  virtual Midi_item* midi_item_p() = 0;

  Audio_column* audio_column_l_;
  /*
    THIS SUX. This ties the output system to the input system.  Bad move.
   */
  Request* req_l_;

  DECLARE_MY_RUNTIME_TYPEINFO;    
protected:
  virtual void do_print () const;
  
private:
  Audio_item (Audio_item const&);
  Audio_item& operator=( Audio_item const&);
};

struct Audio_key : public Audio_item {
  Audio_key (Request* req_l);
  DECLARE_MY_RUNTIME_TYPEINFO;
  virtual Midi_item* midi_item_p();
};

struct Audio_instrument : public Audio_item {
  Audio_instrument (String instrument_str);
  virtual Midi_item* midi_item_p();
  String str_;
    DECLARE_MY_RUNTIME_TYPEINFO;
};
                                      
struct Audio_note : public Audio_item {
  DECLARE_MY_RUNTIME_TYPEINFO;
  Audio_note (Request* req_l);
  virtual Midi_item* midi_item_p();
};

struct Audio_text : Audio_item {
  enum Type { 
    TEXT = 1, COPYRIGHT, TRACK_NAME, INSTRUMENT_NAME, LYRIC, 
    MARKER, CUE_POINT
  };
  DECLARE_MY_RUNTIME_TYPEINFO;
  Audio_text (Audio_text::Type type, String text_str);
  virtual Midi_item* midi_item_p();

  Type type_;
  String text_str_;
};

struct Audio_tempo : Audio_item {
  Audio_tempo (int per_minute_4_i);
  virtual Midi_item* midi_item_p();
  DECLARE_MY_RUNTIME_TYPEINFO;
  int per_minute_4_i_;
};

struct Audio_meter : Audio_item {
  Audio_meter (Request* req_l);
  virtual Midi_item* midi_item_p();
  DECLARE_MY_RUNTIME_TYPEINFO;
};

#endif // AUDIO_ITEM_HH

