/*
  key-performer.cc -- implement Key_performer

  source file of the GNU LilyPond music typesetter

  (c) 1997 Jan Nieuwenhuizen <jan@digicash.com>
*/

#include "key-performer.hh"
#include "command-request.hh"
#include "audio-item.hh"



IMPLEMENT_IS_TYPE_B1(Key_performer,Performer);
ADD_THIS_PERFORMER(Key_performer);

Key_performer::Key_performer()
{
    key_req_l_ = 0;
}

Key_performer::~Key_performer()
{
}

void 
Key_performer::do_print() const
{
#ifndef NPRINT
    if ( key_req_l_ )
    	key_req_l_->print();
#endif
}

void
Key_performer::process_requests()
{
    if ( key_req_l_ )
	play( new Audio_key( key_req_l_ ) );
    key_req_l_ = 0;
}

bool
Key_performer::do_try_request( Request* req_l )
{
    if ( key_req_l_ )
	return false;

    if ( req_l->command() )
	key_req_l_ = req_l->command()->keychange();

    if ( key_req_l_ )
	return true;

    return false;
}

