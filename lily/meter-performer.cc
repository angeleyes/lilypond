/*
  meter-performer.cc -- implement Meter_performer

  source file of the GNU LilyPond music typesetter

  (c) 1997 Jan Nieuwenhuizen <jan@digicash.com>
*/

#include "meter-performer.hh"
#include "command-request.hh"
#include "midi-item.hh"



IMPLEMENT_IS_TYPE_B1(Meter_performer,Performer);
ADD_THIS_PERFORMER(Meter_performer);

Meter_performer::Meter_performer()
{
    meter_req_l_ = 0;
}

Meter_performer::~Meter_performer()
{
}

void 
Meter_performer::do_print() const
{
#ifndef NPRINT
    if ( meter_req_l_ )
    	meter_req_l_->print();
#endif
}

void
Meter_performer::process_requests()
{
    if ( meter_req_l_ ) {
	Midi_time m( meter_req_l_->beats_i_, meter_req_l_->one_beat_i_, 18 );
	play_event( &m );
	meter_req_l_ = 0;
    }
}

bool
Meter_performer::do_try_request( Request* req_l )
{
    if ( meter_req_l_ )
	return false;

    if ( req_l->command() )
	meter_req_l_ = req_l->command()->meterchange();

    if ( meter_req_l_ )
	return true;

    return false;
}

