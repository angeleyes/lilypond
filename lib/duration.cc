/*
  duration.cc -- implement Duration, Plet, 

  source file of the LilyPond music typesetter

  (c) 1997 Jan Nieuwenhuizen <jan@digicash.com>
           Han-Wen Nienhuys <hanwen@stack.nl>
*/

#include "proto.hh"
#include "plist.hh"
#include "string.hh"
#include "source-file.hh"
#include "source.hh"
#include "moment.hh"
#include "duration.hh"
#include "duration-convert.hh"

// statics Duration
int Duration::division_1_i_s = 384 * 4;


Duration::Duration()
{
  durlog_i_ = 0;
  dots_i_ = 0;
  ticks_i_ = 0;
}

Duration::Duration( int type_i, int dots_i = 0 )
{
  durlog_i_ = type_i;
  dots_i_ = dots_i;
  ticks_i_ = 0;
}

bool
Duration::duration_type_b(int t)
{
  /*
    ugh. Assuming behavior of conversion funcs on broken input.
   */
  return t == Duration_convert::type2_i( Duration_convert::i2_type(t));
}

// ugh, what's this?
// i should be called "mom()", ... or at least "length_mom()"
Moment
Duration::length() const
{
  return Duration_convert::dur2_mom(*this);
}

void
Duration::set_plet(int i, int t)
{
  plet_.iso_i_ = i; 
  plet_.type_i_ = t;
}

void
Duration::set_plet(Duration d)
{
  plet_.iso_i_ = d.plet_.iso_i_; 
  plet_.type_i_ = d.plet_.type_i_;
}

void
Duration::set_ticks( int ticks_i )
{
  assert( durlog_i_ <10 );
  assert( !dots_i_ );
  ticks_i_ = ticks_i;
}

String
Duration::str()const
{
  return Duration_convert::dur2_str(*this);
}


bool
Duration::plet_b()
{
  return !plet_.unit_b();
}
