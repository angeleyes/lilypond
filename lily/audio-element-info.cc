/*
  Audio-element-info.cc -- implement Audio_element_info

  source file of the GNU LilyPond music typesetter

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "audio-element-info.hh"

#include "translator-group.hh"
#include "context.hh"

Audio_element_info::Audio_element_info (Audio_element*s, Music *r)
{
  elem_ = s;
  origin_trans_ = 0;
  event_ = r;
}


Audio_element_info::Audio_element_info ()
{
  elem_ = 0;
  event_ = 0;
  origin_trans_ = 0;
}


Link_array<Context>
Audio_element_info::origin_contexts (Translator* end) const
{
  Context * t = origin_trans_->context ();
  Link_array<Context> r;
  do {
    r.push (t);
    t = t->get_parent_context ();
  } while (t && t != end->context ());
  
  return r;
}
