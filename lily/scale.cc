/* 
  scale.cc -- implement Scale
  
  source file of the GNU LilyPond music typesetter
  
  (c) 2006--2007 Han-Wen Nienhuys <hanwen@lilypond.org>
  2008 Rune Zedeler <rz@daimi.au.dk>
*/

#include "scale.hh"

#include "ly-smobs.icc"

/*
  todo: put string <-> pitch here too.

*/
LY_DEFINE (ly_make_scale, "ly:make-scale",
	   2, 0, 0, (SCM steps, SCM orders),
	   "Create a scale.  Takes a vector of integers as argument.")
{
  bool type1_ok = scm_is_vector (steps);
  bool type2_ok = scm_is_vector (orders);

  vector<Rational> semitones; 
  vector<int> keys; 
  if (type1_ok && type2_ok)
    {
      int len = scm_c_vector_length (steps);
      for (int i = 0 ; i < len; i++)
	{
 	  SCM step = scm_c_vector_ref (steps, i);
	  type1_ok = type1_ok && scm_is_rational (step);
	  if (type1_ok)
	    {
	      Rational from_c (scm_to_int (scm_numerator (step)),
			       scm_to_int (scm_denominator (step)));
	      semitones.push_back (from_c);
	    }
	  SCM order = scm_c_vector_ref (orders, i);
	  type2_ok = type2_ok && scm_is_integer (order);
	  if (type2_ok)
	    {
	      int from_c = scm_to_int (order);
	      keys.push_back (from_c);
	    }
	}
    }
  
  
  SCM_ASSERT_TYPE (type1_ok, steps, SCM_ARG1, __FUNCTION__, "vector of rational");
  SCM_ASSERT_TYPE (type2_ok, orders, SCM_ARG2, __FUNCTION__, "vector of int");

  Scale *s = new Scale;
  s->step_tones_ = semitones;
  s->step_order_ = keys;

  SCM retval =  s->self_scm ();

  s->unprotect ();
  
  return retval;
}

LY_DEFINE (ly_default_scale, "ly:default-scale",
	   0, 0, 0, (),
	   "Get the global default scale.")
{
  return default_global_scale
    ? SCM_BOOL_F
    : default_global_scale->self_scm ();
}


Scale * default_global_scale = 0;

LY_DEFINE (ly_set_default_scale, "ly:set-default-scale",
	   1, 0, 0, (SCM scale),
	   "Set the global default scale.")
{
  LY_ASSERT_SMOB (Scale, scale, 1);

  Scale *s = Scale::unsmob (scale);
  if (default_global_scale)
    default_global_scale->unprotect ();
  default_global_scale = s;
  s->protect ();
  
  return SCM_UNSPECIFIED;
}

Rational
Scale::pitch_at_step (int notename)
{
  return pitch_at_step (0, notename);
}

Rational
Scale::pitch_at_step (int octave, int notename)
{
  //  cout << "pitch@" << octave << "," << notename << ":";
  int notenamei = notename % (int)step_tones_.size ();
  //  cout << "("<<step_tones_.size ()<<")";
  if (notenamei<0)
    {
      notenamei += step_tones_.size ();
    }
  //  cout << "("<<notenamei<<")";
  octave += (notename-notenamei) / (int)step_tones_.size ();
  Rational result(step_tones_[notenamei] + Rational (6*octave));
  //  cout << result.to_string() << " ";
  //cout << endl << "(-1)%7 = " << (-1)%(int)step_tones_.size () << endl;
  return result;
}

int
Scale::order_steps(int stepa, int stepb)
{
  return step_order_[stepb] - step_order_[stepa];
}

int
Scale::print_smob (SCM x, SCM port, scm_print_state *)
{
  (void) x;
  
  scm_puts ("#<Scale>", port); 
  return 1;
}


SCM
Scale::mark_smob (SCM x)
{
  (void) x;
  return SCM_UNSPECIFIED;
}

Scale::Scale ()
{
  smobify_self ();
}

Scale::Scale (Scale const &src)
{
  step_tones_ = src.step_tones_;
  smobify_self ();
}


Scale::~Scale ()
{
}

IMPLEMENT_SMOBS (Scale);
IMPLEMENT_DEFAULT_EQUAL_P (Scale);
