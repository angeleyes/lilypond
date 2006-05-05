/*
  listener.hh -- declare Listener

  source file of the GNU LilyPond music typesetter

  (c) 2005 Erik Sandberg <mandolaerik@gmail.com>
*/

#ifndef LISTENER_HH
#define LISTENER_HH

/*
  Listeners

  Listeners are used for stream event dispatching. If you want to
  register a method as an event handler in a dispatcher, then you
  must:

  - declare the method using the DECLARE_LISTENER macro. 
  class Foo
  {
    DECLARE_LISTENER (method);
    ...
  };
  This macro declares the method to take a SCM as parameter, and to
    return void. It also declares some other stuff that shouldn't be
    touched.

  - implement the method using IMPLEMENT_LISTENER:
  IMPLEMENT_LISTENER (Foo, method)
  void method (SCM e)
  {
    write ("Foo hears an event!");
  }

  - Extract a listener using GET_LISTENER (Foo->method)
  - Register the method to the dispatcher using Dispatcher::register

  Example:

  Foo *foo = (...);
  Stream_distributor *d = (...);
  Listener l = GET_LISTENER (foo->method);
  d->register_listener (l, "EventClass");
  
  Whenever d hears a stream-event ev of class "EventClass",
  the implemented procedure is called.

  Limitations:
  - DECLARE_LISTENER currently only works inside smob classes.
*/

#include "smobs.hh"

typedef struct {
  void (*listen_callback) (void *, SCM);
  void (*mark_callback) (void *);
} Listener_function_table;

class Listener {
  void *target_;
  Listener_function_table *type_;
public:
  Listener (const void *target, Listener_function_table *type);
  Listener (Listener const &other);
  void listen (SCM ev) const;

  bool operator == (Listener const &other) const
  { return target_ == other.target_ && type_ == other.type_; }

  DECLARE_SIMPLE_SMOBS (Listener,);
};
DECLARE_UNSMOB (Listener, listener);

#define IMPLEMENT_LISTENER(cl, method)			        \
void								\
cl :: method ## _callback (void *self, SCM ev)	                \
{								\
  cl *s = (cl *)self;						\
  s->method (ev);					        \
}								\
void								\
cl :: method ## _mark (void *self) 				\
{								\
  cl *s = (cl *)self;						\
  scm_gc_mark (s->self_scm ());					\
}								\
Listener							\
cl :: method ## _listener () const				\
{								\
  static Listener_function_table callbacks;			\
  callbacks.listen_callback = &cl::method ## _callback;		\
  callbacks.mark_callback = &cl::method ## _mark;		\
  return Listener (this, &callbacks);				\
}

#define GET_LISTENER(proc) ( proc ## _listener ())

#define DECLARE_LISTENER(name)				\
  inline void name (SCM);		       	        \
  static void name ## _callback (void *self, SCM ev);	\
  static void name ## _mark (void *self);		\
  Listener name ## _listener () const

#endif /* LISTENER_HH */
