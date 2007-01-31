/*
  stream-event.cc -- implement Scheme bindings for Stream_event

  source file of the GNU LilyPond music typesetter

  (c) 2006--2007 Erik Sandberg  <mandolaerik@gmail.com>
*/

#include "stream-event.hh"

LY_DEFINE (ly_make_stream_event, "ly:make-stream-event",
	   1, 1, 0, (SCM cl, SCM proplist),
	   "Creates a stream event of class @var{cl} with the given mutable property list.\n" )
{
  LY_ASSERT_TYPE (ly_is_symbol, cl, 1);

  /* should be scm_list_p, but scm_list_p is expensive. */
  LY_ASSERT_TYPE (scm_is_pair, proplist, 2);
  
  if (proplist == SCM_UNDEFINED)
    proplist = SCM_EOL;

  Stream_event *e = new Stream_event (cl, proplist);
  return e->unprotect ();
}

LY_DEFINE (ly_event_property, "ly:event-property", 
           2, 0, 0, (SCM sev, SCM sym),
	   "Get the property @var{sym} of stream event @var{mus}.\n"
	   "If @var{sym} is undefined, return @code{'()}.\n")
{
  LY_ASSERT_SMOB (Stream_event, sev, 1);
  LY_ASSERT_TYPE (ly_is_symbol, sym, 2);

  Stream_event *e = unsmob_stream_event (sev);

  return e->internal_get_property (sym);
}

LY_DEFINE (ly_event_set_property_x, "ly:event-set-property!",
           3, 0, 0, (SCM ev, SCM sym, SCM val),
           "Set property @var{sym} in event @var{ev} to @var{val}.")
{
  LY_ASSERT_SMOB (Stream_event, ev, 1);
  LY_ASSERT_TYPE (ly_is_symbol, sym, 2);
  
  return ly_prob_set_property_x (ev, sym, val);
}

LY_DEFINE (ly_event_deep_copy, "ly:event-deep-copy",
	   1, 0, 0, (SCM m),
	   "Copy @var{m} and all sub expressions of @var{m}")
{
  SCM copy = m;
  if (Stream_event *ev = unsmob_stream_event (m))
    {
      ev = ev->clone ();
      copy = ev->unprotect ();
    }
  else if (scm_is_pair (m))
    copy = scm_cons (ly_event_deep_copy (scm_car (m)),
		     ly_event_deep_copy (scm_cdr (m)));

  return copy;
}
