/*
  spring.cc -- declare Spring

  source file of the GNU LilyPond music typesetter

  (c) 2007 Joe Neeman <joeneeman@gmail.com>
*/

#include "spring.hh"

Spring::Spring ()
{
  distance_ = 1.0;
  min_distance_ = 1.0;
  inverse_stretch_strength_ = 1.0;
  inverse_compress_strength_ = 1.0;

  update_blocking_force ();
}

Spring::Spring (Real dist, Real min_dist)
{
  distance_ = 1.0;
  min_distance_ = 1.0;

  set_distance (dist);
  set_min_distance (min_dist);
  set_default_strength ();
  update_blocking_force ();
}

void
Spring::update_blocking_force ()
{
  if (distance_ == min_distance_)
    blocking_force_ = 0.0;
  else
    blocking_force_ = (min_distance_ - distance_) / inverse_compress_strength_;
}

/* scale a spring, but in a way that doesn't violate min_distance */
void
Spring::operator*= (Real r)
{
  distance_ = max (min_distance_, distance_ * r);
  inverse_compress_strength_ = distance_ - min_distance_;
  inverse_stretch_strength_ *= 0.8;
}

bool
Spring::operator> (Spring const &other) const
{
  return blocking_force_ > other.blocking_force_;
}

/* merge springs, basically by averaging them, but leave a little headroom
   above the largest minimum distance so that things don't get too cramped */
Spring
merge_springs (vector<Spring> const &springs)
{
  Real avg_distance = 0;
  Real min_distance = 0;
  Real avg_stretch = 0;

  for (vsize i = 0; i < springs.size (); i++)
    {
      avg_distance += springs[i].distance ();
      avg_stretch += springs[i].inverse_stretch_strength ();
      min_distance = max (springs[i].min_distance (), min_distance);
    }

  avg_stretch /= springs.size ();
  avg_distance /= springs.size ();
  avg_distance = max (min_distance + 0.3, avg_distance);

  Spring ret = Spring (avg_distance, min_distance);
  ret.set_inverse_stretch_strength (avg_stretch);

  return ret;
}

void
Spring::set_distance (Real d)
{
  if (d < 0 || isinf (d) || isnan (d))
    programming_error ("insane spring distance requested, ignoring it");
  else
    {
      min_distance_ = min (min_distance_, d);
      distance_ = d;
      update_blocking_force ();
    }
}

void
Spring::set_min_distance (Real d)
{
  if (d < 0 || isinf (d) || isnan (d))
    programming_error ("insane spring min_distance requested, ignoring it");
  else
    {
      min_distance_ = d;
      distance_ = max (distance_, min_distance_);
      update_blocking_force ();
    }
}

void
Spring::set_inverse_stretch_strength (Real f)
{
  if (isinf (f) || isnan (f) || f < 0)
    programming_error ("insane spring constant");
  else
    inverse_stretch_strength_ = f;
}

void
Spring::set_inverse_compress_strength (Real f)
{
  if (isinf (f) || isnan (f) || f < 0)
    programming_error ("insane spring constant");
  else
    inverse_compress_strength_ = f;

  update_blocking_force ();
}

void
Spring::set_blocking_force (Real f)
{
  if (isinf (f) || isnan (f))
    {
      programming_error ("insane blocking force");
      return;
    }

  blocking_force_ = -infinity_f;
  min_distance_ = length (f);
  distance_ = max (distance_, min_distance_);
  update_blocking_force ();
}

void
Spring::set_default_strength ()
{
  inverse_compress_strength_ = distance_ - min_distance_;
  inverse_stretch_strength_ = distance_ - min_distance_;
}

Real
Spring::length (Real f) const
{
  Real force = max (f, blocking_force_);
  Real inv_k = force < 0.0 ? inverse_compress_strength_ : inverse_stretch_strength_;
  return distance_ + force * inv_k;
}
