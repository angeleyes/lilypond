/*
  global-context.hh -- declare Global_context

  source file of the GNU LilyPond music typesetter

  (c) 1997--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/


#ifndef GLOBAL_CONTEXT_HH
#define GLOBAL_CONTEXT_HH

#include "context.hh"
#include "pqueue.hh"

class Global_context : public virtual Context
{
  PQueue<Moment> extra_mom_pq_;
  Music_output_def*  output_def_;
  
  friend class Music_output_def;
public:
  Global_context (Music_output_def*, Moment final);
  int get_moments_left () const;
  Moment sneaky_insert_extra_moment (Moment);
  void add_moment_to_process (Moment);
  void run_iterator_on_me (Music_iterator*);
  virtual Score_context*get_score_context () const;
  
  void apply_finalizations ();
  void add_finalization (SCM);

  virtual Music_output *get_output ();     
  virtual void prepare (Moment);
  virtual void one_time_step ();
  virtual void finish ();
  virtual Music_output_def* get_output_def () const; 
  virtual Moment now_mom () const;
  virtual Context *get_default_interpreter ();


  Moment previous_moment () const;
protected:
  Moment final_mom_;
  Moment prev_mom_;
  Moment now_mom_;
};



#endif // GLOBAL_CONTEXT_HH
