/*
  lily-guile.cc -- implement assorted guile functions

  source file of the GNU LilyPond music typesetter

  (c) 1998--2004 Jan Nieuwenhuizen <janneke@gnu.org>

  Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>   /* isinf */
#include <string.h> /* strdup, strchr */
#include <ctype.h>

#include "lily-proto.hh"
#include "version.hh"

/* MacOS S fix:
   source-file.hh includes cmath which undefines isinf and isnan

   FIXME: #ifdef MACOS_X?
*/
inline int my_isinf (Real r) { return isinf (r); }
inline int my_isnan (Real r) { return isnan (r); }


#include "libc-extension.hh"
#include "lily-guile.hh"
#include "main.hh"
#include "file-path.hh"
#include "warn.hh"
#include "direction.hh"
#include "offset.hh"
#include "interval.hh"
#include "pitch.hh"
#include "dimensions.hh"
#include "source-file.hh"

// #define TEST_GC

SCM
ly_last (SCM list)
{
  return ly_car (scm_last_pair (list));
}


SCM
ly_write2scm (SCM s)
{
  SCM port = scm_mkstrport (SCM_INUM0, 
			    scm_make_string (SCM_INUM0, SCM_UNDEFINED),
			    SCM_OPN | SCM_WRTNG,
			    "ly_write2string");
  //  SCM write = scm_eval_3 (ly_symbol2scm ("write"), s, SCM_EOL);
  SCM write = scm_primitive_eval (ly_symbol2scm ("write"));
  
  // scm_apply (write, port, SCM_EOL);
  gh_call2 (write, s, port);
  return scm_strport_to_string (port);
}


SCM
ly_quote_scm (SCM s)
{
  return scm_list_n (ly_symbol2scm ("quote"), s, SCM_UNDEFINED);
}

String
ly_symbol2string (SCM s)
{
  assert (gh_symbol_p (s));
  return String ((Byte*)SCM_STRING_CHARS (s), (int) SCM_STRING_LENGTH (s));
}

String
gulp_file_to_string (String fn)
{
  String s = global_path.find (fn);
  if (s == "")
    {
      String e = _f ("can't find file: `%s'", fn);
      e += " ";
      e += _f ("(load path: `%s')", global_path.to_string ());
      error (e);
    }
  else if (verbose_global_b)
    progress_indication ("[" + s);

  int n;
  char * str = gulp_file (s, &n);
  String result (str);
  delete[] str;
  
  if (verbose_global_b)
    progress_indication ("]");

  return result;
}

LY_DEFINE (ly_gulp_file, "ly:gulp-file",
	   1, 0, 0, (SCM name),
	   "Read the file @var{name}, and return its contents in a string.  "
	   "The file is looked up using the search path.")
{
  return scm_makfrom0str (gulp_file_to_string (ly_scm2string (name)).to_str0 ());
}


extern "C" {
  // maybe gdb 5.0 becomes quicker if it doesn't do fancy C++ typing?
void
ly_display_scm (SCM s)
{
  gh_display (s);
  gh_newline ();
}
};

String
ly_scm2string (SCM s)
{
  assert (gh_string_p (s));

  char *p = SCM_STRING_CHARS (s);
  String r (p);
  return r;
}

SCM
index_get_cell (SCM s, Direction d)
{
  
  assert (d);
  return (d == LEFT) ? ly_car (s) : ly_cdr (s);
}

SCM
index_set_cell (SCM s, Direction d, SCM v)
{
  if (d == LEFT)
    gh_set_car_x (s, v);
  else if (d == RIGHT)
    gh_set_cdr_x (s, v);
  return s;
}
  
LY_DEFINE (ly_warning,"ly:warn", 1, 0, 0,
  (SCM str), "Scheme callable function to issue the warning @code{msg}.")
{
  SCM_ASSERT_TYPE (gh_string_p (str), str, SCM_ARG1, __FUNCTION__, "string");
  progress_indication ("\n");
  warning ("lily-guile: " + ly_scm2string (str));
  return SCM_BOOL_T;
}

LY_DEFINE (ly_dir_p,  "ly:dir?", 1,0, 0,  (SCM s),
	  "type predicate. A direction is @code{-1}, @code{0} or "
	   "@code{1}, where @code{-1} represents "
	  "left or down and @code{1} represents right or up.")
{
  if (gh_number_p (s))
    {
      int i = gh_scm2int (s);
      return (i>= -1 && i <= 1)  ? SCM_BOOL_T : SCM_BOOL_F; 
    }
  return SCM_BOOL_F;
}

bool
is_number_pair (SCM p)
{
  return gh_pair_p (p) && gh_number_p (ly_car (p)) && gh_number_p (ly_cdr (p));
}

typedef void (*Void_fptr) ();
Array<Void_fptr> *scm_init_funcs_;

void add_scm_init_func (void (*f) ())
{
  if (!scm_init_funcs_)
    scm_init_funcs_ = new Array<Void_fptr>;

  scm_init_funcs_->push (f);
}


void
ly_init_ly_module (void *)
{
  for (int i=scm_init_funcs_->size () ; i--;)
    (scm_init_funcs_->elem (i)) ();

  if (verbose_global_b)
    progress_indication ("\n");
  
  scm_primitive_load_path (scm_makfrom0str ("lily.scm"));
}


SCM global_lily_module;

void
ly_init_guile ()
{
  global_lily_module = scm_c_define_module ("lily", ly_init_ly_module, 0);
  scm_c_use_module ("lily");
}

unsigned int ly_scm_hash (SCM s)
{
  return scm_ihashv (s, ~1u);
}



bool
is_direction (SCM s)
{
  if (gh_number_p (s))
    {
      int i = gh_scm2int (s);
      return i>= -1 && i <= 1; 
    }
  return false;
}


bool
is_axis (SCM s)
{
  if (gh_number_p (s))
    {
      int i = gh_scm2int (s);
      return i== 0 || i == 1;
    }
  return false;
}

Direction
to_dir (SCM s)
{
  return SCM_INUMP (s) ? (Direction) gh_scm2int (s) : CENTER;
}

Interval
ly_scm2interval (SCM p)
{
  return Interval (gh_scm2double (ly_car (p)), gh_scm2double (ly_cdr (p)));
}

Drul_array<Real>
ly_scm2realdrul (SCM p)
{
  return Drul_array<Real> (gh_scm2double (ly_car (p)),
			   gh_scm2double (ly_cdr (p)));
}

SCM
ly_interval2scm (Drul_array<Real> i)
{
  return gh_cons (gh_double2scm (i[LEFT]), gh_double2scm (i[RIGHT]));
}

bool
to_boolean (SCM s)
{
  return gh_boolean_p (s) && gh_scm2bool (s);
}

/* Appendable list L: the cdr contains the list, the car the last cons
   in the list.  */
SCM
appendable_list ()
{
  SCM s = gh_cons (SCM_EOL, SCM_EOL);
  gh_set_car_x (s, s);
  
  return s;
}

void
appendable_list_append (SCM l, SCM elt)
{
  SCM newcons = gh_cons (elt, SCM_EOL);
  
  gh_set_cdr_x (ly_car (l), newcons);      
  gh_set_car_x (l, newcons);
}


SCM
ly_offset2scm (Offset o)
{
  return gh_cons (gh_double2scm (o[X_AXIS]), gh_double2scm (o[Y_AXIS]));
}

Offset
ly_scm2offset (SCM s)
{
  return Offset (gh_scm2double (ly_car (s)),
		 gh_scm2double (ly_cdr (s)));
}

   
LY_DEFINE (ly_number2string, "ly:number->string",
	   1, 0, 0, (SCM s),
	   "Convert @var{num} to a string without generating many decimals.")
{
  SCM_ASSERT_TYPE (gh_number_p (s), s, SCM_ARG1, __FUNCTION__, "number");

  char str[400];			// ugh.

  if (scm_exact_p (s) == SCM_BOOL_F)
    {
      Real r (gh_scm2double (s));

      if (my_isinf (r) || my_isnan (r))
	{
	  programming_error ("Infinity or NaN encountered while converting Real number; setting to zero.");
	  r = 0.0;
	}

      sprintf (str, "%08.4f", r);
    }
  else
    sprintf (str, "%d", gh_scm2int (s));

  return scm_makfrom0str (str);
}

/*
  Undef this to see if GUILE GC is causing too many swaps.
 */

//#define TEST_GC

#ifdef TEST_GC
#include <libguile/gc.h>

static void *
greet_sweep (void *dummy1, void *dummy2, void *dummy3)
{
  fprintf (stderr, "entering sweep\n");
}

static void *
wave_sweep_goodbye (void *dummy1, void *dummy2, void *dummy3)
{
  fprintf (stderr, "leaving sweep\n");
}
#endif


LY_DEFINE (ly_version,  "ly:version", 0, 0, 0, (),
	  "Return the current lilypond version as a list, e.g. @code{(1 3 127 uu1)}. ")
{
  char const* vs = "\'(" MAJOR_VERSION " " MINOR_VERSION " "  PATCH_LEVEL " " MY_PATCH_LEVEL ")" ;
  
  return gh_eval_str ((char*)vs);
}

LY_DEFINE (ly_unit,  "ly:unit", 0, 0, 0, (),
	  "Return the unit used for lengths as a string.")
{
  return scm_makfrom0str (INTERNAL_UNIT);
}



LY_DEFINE (ly_dimension_p,  "ly:dimension?", 1, 0, 0, (SCM d),
	  "Return @var{d} is a number. Used to distinguish length "
	  "variables from normal numbers.")
{
  return scm_number_p (d);
}

static void
init_functions ()
{
#ifdef TEST_GC 
  scm_c_hook_add (&scm_before_mark_c_hook, greet_sweep, 0, 0);
  scm_c_hook_add (&scm_before_sweep_c_hook, wave_sweep_goodbye, 0, 0);
#endif
}

ADD_SCM_INIT_FUNC (funcs, init_functions);

SCM
ly_deep_copy (SCM src)
{
  if (gh_pair_p (src))
    return gh_cons (ly_deep_copy (ly_car (src)), ly_deep_copy (ly_cdr (src)));
  else if (gh_vector_p (src))
    {
      int len = SCM_VECTOR_LENGTH (src);
      SCM nv = scm_c_make_vector (len, SCM_UNDEFINED);
      for (int i  =0 ; i < len ; i++)
	{
	  SCM si = gh_int2scm (i);
	  scm_vector_set_x (nv, si, ly_deep_copy (scm_vector_ref (src, si))); 
	}
    }
  return src;
}




SCM
ly_assoc_chain (SCM key, SCM achain)
{
  if (gh_pair_p (achain))
    {
      SCM handle = scm_assoc (key, ly_car (achain));
      if (gh_pair_p (handle))
	return handle;
      else
	return ly_assoc_chain (key, ly_cdr (achain));
    }
  else
    return SCM_BOOL_F;
}

/* looks the key up in the cdrs of the alist-keys
   - ignoring the car and ignoring non-pair keys.
   Returns first match found, i.e.

   alist = ((1 . 10)
                   ((1 . 2) . 11)
                   ((2 . 1) . 12)
                   ((3 . 0) . 13)
                   ((4 . 1) . 14) )

I would like (ly_assoc_cdr 1) to return 12 - because it's the first
element with the cdr of the key = 1.  In other words (alloc_cdr key)
corresponds to call

(alloc (anything . key))



*/
SCM
ly_assoc_cdr (SCM key, SCM alist)
{
  if (gh_pair_p (alist))
  {
    SCM trykey = ly_caar (alist);
    if (gh_pair_p (trykey) && to_boolean (scm_equal_p (key, ly_cdr (trykey))))
    return ly_car (alist);
    else
    return ly_assoc_cdr (key, ly_cdr (alist));
  }
  return SCM_BOOL_F;
}

/* LST has the form "sym1 sym2 sym3\nsym4\nsym5"
   i.e. \n and ' ' can be used interchangeably as separators.  */
SCM
parse_symbol_list (char const *lst)
{
  char *s = strdup (lst);
  char *orig = s;
  SCM create_list = SCM_EOL;

  char * e = s + strlen (s) - 1;
  while (e >= s && isspace (*e))
    *e-- = 0;

  for (char * p = s; *p; p++)
    if (*p == '\n')
      *p = ' ';
  
  if (!s[0])
    s = 0;
  
  while (s)
    {
      char *next = strchr (s, ' ');
      if (next)
	*next++ = 0;

      create_list = gh_cons (ly_symbol2scm (s), create_list);
      s = next;
    }

  free (orig);
  return create_list;
}

SCM
ly_truncate_list (int k, SCM lst)
{
  if (k == 0)
    lst = SCM_EOL;
  else
    {
      SCM s = lst;
      k--;
      for (; gh_pair_p (s) && k--; s = ly_cdr (s))
	;

      if (gh_pair_p (s))
	gh_set_cdr_x (s, SCM_EOL);
    }
  return lst;
}

String
print_scm_val (SCM val)
{
  String realval = ly_scm2string (ly_write2scm (val));
  if (realval.length () > 200)
    realval = realval.left_string (100)
      + "\n :\n :\n"
      + realval.right_string (100);
  return realval;	 
}

bool
type_check_assignment (SCM sym, SCM val,  SCM type_symbol) 
{
  bool ok = true;

  /*
    Always succeeds.


    TODO: should remove #f from allowed vals?
   */
  if (val == SCM_EOL || val == SCM_BOOL_F)
    return ok;

  if (!gh_symbol_p (sym))
#if 0
    return false;
#else
  /*
    This is used for autoBeamSettings.

    TODO: deprecate the use of \override and \revert for
    autoBeamSettings?

    or use a symbol autoBeamSettingS?  
   */
  return true; 
#endif
  
  SCM type = scm_object_property (sym, type_symbol);

  if (type != SCM_EOL && !gh_procedure_p (type))
      {
	warning (_f ("Can't find property type-check for `%s' (%s).",
		     ly_symbol2string (sym).to_str0 (),
		     ly_symbol2string (type_symbol).to_str0 ())
		 + "  " + _ ("Perhaps you made a typing error?"));

	/* Be strict when being anal :) */
	if (internal_type_checking_global_b)
	  abort ();
	
	warning (_ ("Doing assignment anyway."));
      }
  else
    {
      if (val != SCM_EOL
	  && gh_procedure_p (type)
	  && gh_call1 (type, val) == SCM_BOOL_F)
	{
	  SCM errport = scm_current_error_port ();
	  ok = false;
	  SCM typefunc = ly_scheme_function ("type-name");
	  SCM type_name = gh_call1 (typefunc, type);

	 
	  scm_puts (_f ("Type check for `%s' failed; value `%s' must be of type `%s'",
			ly_symbol2string (sym).to_str0 (),
			print_scm_val (val),
			ly_scm2string (type_name).to_str0 ()).to_str0 (),
		    errport);
	  scm_puts ("\n", errport);		      
	}
    }
  return ok;
}


/* some SCM abbrevs

   zijn deze nou handig?
   zijn ze er al in scheme, maar heten ze anders? */


/* Remove doubles from (sorted) list */
SCM
ly_unique (SCM list)
{
  SCM unique = SCM_EOL;
  for (SCM i = list; gh_pair_p (i); i = ly_cdr (i))
    {
      if (!gh_pair_p (ly_cdr (i))
	  || !gh_equal_p (ly_car (i), ly_cadr (i)))
	unique = gh_cons (ly_car (i), unique);
    }
  return scm_reverse_x (unique, SCM_EOL);
}

/* tail add */
SCM
ly_snoc (SCM s, SCM list)
{
  return gh_append2 (list, scm_list_n (s, SCM_UNDEFINED));
}

/* Split list at member s, removing s.
   Return (BEFORE . AFTER)  */
SCM
ly_split_list (SCM s, SCM list)
{
  SCM before = SCM_EOL;
  SCM after = list;
  for (; gh_pair_p (after);)
    {
      SCM i = ly_car (after);
      after = ly_cdr (after);
      if (gh_equal_p (i, s))
	break;
      before = gh_cons (i, before);
    }
  return gh_cons ( scm_reverse_x (before, SCM_EOL),  after);
  
}


void
taint (SCM *)
{
  /*
    nop.
   */
}

/*
  display stuff without using stack
 */
SCM
display_list (SCM s)
{
  SCM p = scm_current_output_port ();

  scm_puts ("(", p);
  for (; gh_pair_p (s); s =gh_cdr (s))
    {
      scm_display (gh_car (s), p);
      scm_puts (" ", p);      
    }
  scm_puts (")", p);
  return SCM_UNSPECIFIED;
}

Slice
int_list_to_slice (SCM l)
{
  Slice s;
  s.set_empty ();
  for (; gh_pair_p (l); l = gh_cdr (l))
    if (gh_number_p (gh_car (l)))
      s.add_point (gh_scm2int (gh_car (l))); 
  return s;
}

/* Return I-th element, or last elt L. If I < 0, then we take the first
   element.
   
   PRE: length (L) > 0  */
SCM
robust_list_ref (int i, SCM l)
{
  while (i-- > 0 && gh_pair_p (gh_cdr (l)))
    l = gh_cdr (l);
  return gh_car (l);
}

Real
robust_scm2double (SCM k, double x)
{
  if (gh_number_p (k))
    x = gh_scm2double (k);
  return x;
}

Interval
robust_scm2interval (SCM k, Drul_array<Real> v)
{
  Interval i;
  i[LEFT]= v[LEFT];
  i[RIGHT]= v[RIGHT];
  if (is_number_pair (k))
    i = ly_scm2interval (k);
  return i;
}

Drul_array<Real>
robust_scm2drul (SCM k, Drul_array<Real> v)
{
  if (is_number_pair (k))
    v = ly_scm2interval (k);
  return v;
}

Offset
robust_scm2offset (SCM k, Offset o)
{
  if (is_number_pair (k))
    o = ly_scm2offset (k);
  return o;
}

int
robust_scm2int (SCM k, int o)
{
  if (scm_integer_p (k) == SCM_BOOL_T)
    o = gh_scm2int (k);
  return o;
}

SCM
alist_to_hashq (SCM alist)
{
  int i = scm_ilength (alist);
  if (i < 0)
    return scm_make_vector (gh_int2scm (0), SCM_EOL);
	  
  SCM tab = scm_make_vector (gh_int2scm (i), SCM_EOL);
  for (SCM s = alist; gh_pair_p (s); s = ly_cdr (s))
    {
      SCM pt = ly_cdar (s);
      scm_hashq_set_x (tab, ly_caar (s), pt);
    }
  return tab; 
}

#if 1
/*
  Debugging mem leaks:
 */
LY_DEFINE (ly_protects, "ly:protects", 0, 0, 0, (),
	  "Return hash of protected objects.")
{
  return scm_protects;
}
#endif
