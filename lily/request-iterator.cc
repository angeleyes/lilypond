/*
  request-iterator.cc -- implement Request_chord_iterator

  source file of the GNU LilyPond music typesetter

  (c) 1997 Han-Wen Nienhuys <hanwen@stack.nl>
*/

#include "translator.hh"
#include "debug.hh"
#include "request-iterator.hh"
#include "music-list.hh"
#include "request.hh"

IMPLEMENT_IS_TYPE_B1(Request_chord_iterator,Music_iterator);

void
Request_chord_iterator::construct_children()
{
  get_req_translator_l();
}

Request_chord_iterator::Request_chord_iterator (Request_chord*el_l)
{
  elt_l_ = el_l;
  MInterval elt_dur =el_l->time_int();
  elt_duration_ = elt_dur.length (); 
  last_b_ = false;
}


bool
Request_chord_iterator::ok() const
{
  return (elt_duration_ && !last_b_) || first_b_; 
}



Moment
Request_chord_iterator::next_moment() const
{
  Moment m (0);
  if  (!first_b_) 
    m = elt_duration_;
  return m;
}

void
Request_chord_iterator::do_print() const
{
#ifndef NPRINT
  DOUT << "duration: " << elt_duration_;
#endif
}
void
Request_chord_iterator::process_and_next (Moment mom)
{
  if (first_b_) 
    {
      for (PCursor<Music*> i (elt_l_->music_p_list_); i.ok(); i++) 
	{
	  assert (i->is_type_b (Request::static_name()));
	  Request * req_l = (Request*)i.ptr();
	  bool gotcha = report_to_l()->try_request (req_l);
	  if (!gotcha)
	    req_l->warning ("Junking request: " + String (req_l->name()));

	}
      first_b_ = false;
    }

  if (mom >= elt_duration_)
    last_b_ = true;  
}
