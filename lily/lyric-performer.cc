/*
  lyric-performer.cc -- implement Lyric_performer

  source file of the GNU LilyPond music typesetter

  (c) 1997 Jan Nieuwenhuizen <jan@digicash.com>
*/

#include "lyric-performer.hh"
#include "text-def.hh"
#include "musical-request.hh"
#include "audio-item.hh"



IMPLEMENT_IS_TYPE_B1(Lyric_performer,Performer);
ADD_THIS_PERFORMER(Lyric_performer);

Lyric_performer::Lyric_performer()
{
}

Lyric_performer::~Lyric_performer()
{
}

void 
Lyric_performer::do_print() const
{
#ifndef NPRINT
  if ( lreq_arr_.size())
  	lreq_arr_[ 0 ]->print();
#endif
}

void
Lyric_performer::process_requests()
{
  if ( lreq_arr_.size() && lreq_arr_[ 0 ]->tdef_p_->text_str_.length_i() )
	play (new Audio_text (Audio_text::LYRIC, lreq_arr_[ 0 ]->tdef_p_->text_str_) );
  lreq_arr_.clear();
}

bool
Lyric_performer::do_try_request (Request* req_l)
{
  Musical_req* m_l = req_l->musical();
  if ( !m_l || ! m_l->lreq_l()) 
	return false;
  lreq_arr_.push (m_l->lreq_l());

  return true;
}

