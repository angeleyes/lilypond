/*
  audio-item-info.hh -- declare Audio_item_info

  source file of the GNU LilyPond music typesetter

  (c) 1999--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#ifndef AUDIO_ITEM_INFO_HH
#define AUDIO_ITEM_INFO_HH

#include "lily-proto.hh"
#include "parray.hh"

/**
   Data container for broadcasts
*/
class Audio_element_info
{
public:
  Audio_element *elem_;
  Music *event_;
  Translator *origin_trans_;

  Link_array<Context> origin_contexts (Translator *) const;

  Audio_element_info (Audio_element *, Music *);
  Audio_element_info ();
};

#endif
