/*
  Sequential_music_iterator.cc -- implement Sequential_music_iterator

  source file of the GNU LilyPond music typesetter

  (c)  1997--1999 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "translator-group.hh"
#include "debug.hh"
#include "sequential-music-iterator.hh"
#include "music-list.hh"

void
Sequential_music_iterator::do_print() const
{
  if (iter_p_)
    iter_p_->print();
}

Sequential_music_iterator::Sequential_music_iterator ()
{
  cursor_ = 0;
  here_mom_ = 0;
  iter_p_ =0;
}

void
Sequential_music_iterator::construct_children()
{
  cursor_ = dynamic_cast<Sequential_music const*> (music_l_)->music_p_list_p_->head_;
  
  while (cursor_)
    {
      start_next_element();
      if (!iter_p_->ok()) 
	{
	  leave_element();
	}
      else 
	{
	  set_Sequential_music_translator();
	  break;
	}
    }
}

void 
Sequential_music_iterator::leave_element()
{
  delete iter_p_;
  iter_p_ =0;
  Moment elt_time = cursor_->car_->length_mom ();
  here_mom_ += elt_time;
  cursor_ =cursor_->next_;
}

void
Sequential_music_iterator::start_next_element()
{
  assert (!iter_p_);
  iter_p_ = get_iterator_p (cursor_->car_);
}

void
Sequential_music_iterator::set_Sequential_music_translator()
{
  if (iter_p_->report_to_l()->depth_i () > report_to_l ()->depth_i ())
    set_translator (iter_p_->report_to_l());
}

Sequential_music_iterator::~Sequential_music_iterator()
{
  assert (! iter_p_);
}




void
Sequential_music_iterator::do_process_and_next (Moment until)
{
  while (1) 
    {
      Moment local_until = until - here_mom_;
      while (iter_p_->ok()) 
	{
	  Moment here = iter_p_->next_moment();
	  if (here != local_until)
	    goto loopexit;
	    
	  iter_p_->process_and_next (local_until);
	}

      if (!iter_p_->ok()) 
	{
	  leave_element();
	  
	  if (cursor_)
	    {
	      start_next_element();
	      set_Sequential_music_translator();
	    }
	  else 
	    {
	      goto loopexit;
	    }
	}
    }

loopexit:

  Music_iterator::do_process_and_next (until);
}

Moment
Sequential_music_iterator::next_moment() const
{
  return iter_p_->next_moment() + here_mom_;
}

bool
Sequential_music_iterator::ok() const
{
  return iter_p_;
}

