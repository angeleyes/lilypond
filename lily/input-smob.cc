/*   
  input-smob.cc --  implement Input smob
  
  source file of the GNU LilyPond music typesetter
  
  (c) 2000--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  
 */

#include "input-smob.hh"

#include "string.hh"

#include "ly-smobs.icc"

/* Dummy input location for use if real one is missing.  */
Input dummy_input_global;

static long input_tag;

static
SCM mark_smob (SCM)
{
  return SCM_EOL;
}

static int
print_smob (SCM s, SCM port, scm_print_state *)
{
  String str = "#<location " +  unsmob_input (s)->location_string () + ">";
  scm_puts (str.to_str0 (), port);
  return 1;
}

static size_t
free_smob (SCM s)
{
  delete unsmob_input (s);
  return 0;
}


static void
start_input_smobs ()
{
  input_tag = scm_make_smob_type ("input", 0);
  scm_set_smob_mark (input_tag, mark_smob);
  scm_set_smob_free (input_tag, free_smob);
  scm_set_smob_print (input_tag, print_smob);
  scm_set_smob_equalp (input_tag, 0);
}

SCM
make_input (Input ip)
{
  Input *nip = new Input (ip);
  SCM z;
  
  SCM_NEWSMOB (z, input_tag, nip);
  return z;
}

Input *						
unsmob_input (SCM s)
{
  if (SCM_IMP (s))
    return 0;
  if (SCM_CAR (s) == (SCM)input_tag) // ugh.
    return (Input*) SCM_CDR (s);
  else						
    return 0;					
}


ADD_SCM_INIT_FUNC (input, start_input_smobs);

