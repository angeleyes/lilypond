/*
  event.cc -- implement Event

  source file of the GNU LilyPond music typesetter

  (c) 1996--2003 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "event.hh"
#include "warn.hh"
#include "event.hh"
  
Moment
Event::get_length () const
{
  Duration *d = unsmob_duration (get_mus_property ("duration"));
  if (!d)
    {
      Moment m ;
      return m;
    }
  return d->get_length ();
}

void
Event::compress (Moment m)
{
  Duration *d =  unsmob_duration (get_mus_property ("duration"));
  if (d)
    set_mus_property ("duration", d ->compressed (m.main_part_).smobbed_copy ());
}

void
Event::transpose (Pitch delta)
{
  Pitch *p = unsmob_pitch (get_mus_property ("pitch"));
  if (!p)
    return ;

  Pitch np = p->transposed (delta);
  
  if (abs (np.get_alteration ()) > 2)
    {
	warning (_f ("Transposition by %s makes alteration larger than two",
	  delta.string ()));
    }

  set_mus_property ("pitch", np.smobbed_copy ());
}

Pitch
Event::to_relative_octave (Pitch last)
{
  Pitch *old_pit = unsmob_pitch (get_mus_property ("pitch"));
  if (old_pit)
    {
      Pitch new_pit = *old_pit;
      new_pit = new_pit.to_relative_octave (last);
      set_mus_property ("pitch", new_pit.smobbed_copy ());
  
      return new_pit;
    }
  return last;
}
  
Event::Event ()
  : Music ()
{
}

ADD_MUSIC(Event);
LY_DEFINE(music_duration_length, "music-duration-length", 1, 0,0,
	  (SCM mus),
	  "Extract the duration field from @var{mus}, and return the length.")
{
  Music* m =   unsmob_music(mus);
  SCM_ASSERT_TYPE(m, mus, SCM_ARG1, __FUNCTION__, "Music");
  
  Duration *d = unsmob_duration (m->get_mus_property ("duration"));

  Moment l ;
  
  if (d)
    {
      l = d->get_length ();  
    }
  else
    programming_error("Music has no duration");
  return l.smobbed_copy();
  
}


LY_DEFINE(music_duration_compress, "ly:music-duration-compress", 2, 0,0,
	  (SCM mus, SCM factor),
	  "Extract the duration field from @var{mus}, and compress it.")
{
  Music* m =   unsmob_music(mus);
  Moment * f = unsmob_moment (factor);
  SCM_ASSERT_TYPE(m, mus, SCM_ARG1, __FUNCTION__, "Music");
  SCM_ASSERT_TYPE(f, factor, SCM_ARG2, __FUNCTION__, "Moment");
  
  Duration *d = unsmob_duration (m->get_mus_property ("duration"));
  if (d)
    m->set_mus_property ("duration", d->compressed (f->main_part_).smobbed_copy());
  return SCM_UNSPECIFIED;
}



/*
  This is hairy, since the scale in a key-change event may contain
  octaveless notes.


  TODO: this should use ly:pitch. 
 */
LY_DEFINE(transpose_key_alist, "ly:transpose-key-alist",
	  2, 0,0, (SCM l, SCM pitch),
	  "Make a new key alist of @var{l} transposed by pitch @var{pitch}")
{
  SCM newlist = SCM_EOL;
  Pitch *p = unsmob_pitch (pitch);
  
  for (SCM s = l; gh_pair_p (s); s = ly_cdr (s))
    {
      SCM key = ly_caar (s);
      SCM alter = ly_cdar (s);
      if (gh_pair_p (key))
	{
	  Pitch orig (gh_scm2int (ly_car (key)),
		      gh_scm2int (ly_cdr (key)),
		      gh_scm2int (alter));

	  orig =orig.transposed (*p);

	  SCM key = gh_cons (scm_int2num (orig.get_octave ()),
			     scm_int2num (orig.get_notename ()));

	  newlist = gh_cons (gh_cons (key, scm_int2num (orig.get_alteration ())),
			     newlist);
	}
      else if (gh_number_p (key))
	{
	  Pitch orig (0, gh_scm2int (key), gh_scm2int (alter));
	  orig = orig.transposed (*p);

	  key =scm_int2num (orig.get_notename ());
	  alter = scm_int2num (orig.get_alteration());
	  newlist = gh_cons (gh_cons (key, alter), newlist);
	}
    }
  return scm_reverse_x (newlist, SCM_EOL);
}

void
Key_change_ev::transpose (Pitch p)
{
  SCM pa = get_mus_property ("pitch-alist");

  set_mus_property ("pitch-alist", transpose_key_alist (pa, p.smobbed_copy()));
  Pitch tonic = *unsmob_pitch (get_mus_property ("tonic"));
  set_mus_property ("tonic",
		    tonic.smobbed_copy ());
}

bool
alist_equal_p (SCM a, SCM b)
{
  for (SCM s = a;
       gh_pair_p (s); s = ly_cdr (s))
    {
      SCM key = ly_caar (s);
      SCM val = ly_cdar (s);
      SCM l = scm_assoc (key, b);

      if (l == SCM_BOOL_F
	  || !gh_equal_p ( ly_cdr (l), val))

	return false;
    }
  return true;
}
ADD_MUSIC (Key_change_ev);
