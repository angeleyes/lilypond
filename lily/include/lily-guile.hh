/*
  lily-guile.hh encapsulate guile

  source file of the GNU LilyPond music typesetter

  (c) 1998--2005 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#ifndef LILY_GUILE_HH
#define LILY_GUILE_HH

#if __MINGW32__
#include "mingw-compatibility.hh"
#endif

#include <libguile.h>

#include "guile-compatibility.hh"
#include "interval.hh"
#include "lily-guile-macros.hh"
#include "ly-module.hh"

/** Conversion functions follow the GUILE naming convention, i.e.
    A ly_B2A (B b);  */

SCM ly_last (SCM list);
SCM ly_write2scm (SCM s);
SCM ly_deep_copy (SCM);
SCM ly_truncate_list (int k, SCM lst);

SCM ly_to_string (SCM scm);
SCM ly_to_symbol (SCM scm);

extern SCM global_lily_module;

String gulp_file_to_string (String fn, bool must_exist);

String ly_scm2string (SCM s);
String ly_symbol2string (SCM);
SCM ly_offset2scm (Offset);
Offset ly_scm2offset (SCM);
SCM ly_chain_assoc (SCM key, SCM achain);
SCM ly_chain_assoc_get (SCM key, SCM achain, SCM dfault);
SCM ly_assoc_cdr (SCM key, SCM alist);
SCM ly_assoc_get (SCM key, SCM alist, SCM def);
Interval ly_scm2interval (SCM);
Drul_array<Real> ly_scm2realdrul (SCM);
Slice int_list_to_slice (SCM l);
SCM ly_interval2scm (Drul_array<Real>);
char *ly_scm2newstr (SCM str, size_t *lenp);

Real robust_scm2double (SCM, double);
int robust_scm2int (SCM, int);
Drul_array<Real> robust_scm2drul (SCM, Drul_array<Real>);
Interval robust_scm2interval (SCM, Drul_array<Real>);
Offset robust_scm2offset (SCM, Offset);

SCM ly_quote_scm (SCM s);
bool type_check_assignment (SCM val, SCM sym, SCM type_symbol);
String print_scm_val (SCM val);
SCM ly_number2string (SCM s);

SCM parse_symbol_list (char const *);
SCM robust_list_ref (int i, SCM l);
SCM alist_to_hashq (SCM);

SCM ly_alist_vals (SCM alist);
SCM ly_hash2alist (SCM tab);

/* inserts at front, removing dublicates */
inline SCM ly_assoc_front_x (SCM alist, SCM key, SCM val)
{
  return scm_acons (key, val, scm_assoc_remove_x (alist, key));
}
inline bool ly_is_list (SCM x) { return SCM_NFALSEP (scm_list_p (x)); }
inline bool ly_is_procedure (SCM x) { return SCM_NFALSEP (scm_procedure_p (x)); }
inline bool ly_is_port (SCM x) { return SCM_NFALSEP (scm_port_p (x)); }

inline bool ly_is_equal (SCM x, SCM y)
{
  return SCM_NFALSEP (scm_equal_p (x, y));
}

inline bool ly_scm2bool (SCM x) { return SCM_NFALSEP (x); }
inline char ly_scm2char (SCM x) { return SCM_CHAR (x); }
inline unsigned long ly_length (SCM x)
{
  return scm_num2ulong (scm_length (x), 0, "ly_length");
}
inline SCM ly_bool2scm (bool x) { return SCM_BOOL (x); }

inline SCM ly_append2 (SCM x1, SCM x2)
{
  return scm_append (scm_listify (x1, x2, SCM_UNDEFINED));
}
inline SCM ly_append3 (SCM x1, SCM x2, SCM x3)
{
  return scm_append (scm_listify (x1, x2, x3, SCM_UNDEFINED));
}
inline SCM ly_append4 (SCM x1, SCM x2, SCM x3, SCM x4)
{
  return scm_append (scm_listify (x1, x2, x3, x4, SCM_UNDEFINED));
}

/*
  display and print newline.
*/
extern "C" {
  void ly_display_scm (SCM s);
}

void read_lily_scm_file (String);
void ly_c_init_guile ();

bool is_direction (SCM s);
bool is_number_pair (SCM);
bool is_axis (SCM);

/*
  these conversion functions also do a typecheck on the argument, and
  return a default value if S has the wrong type.
*/

Direction to_dir (SCM s);
bool to_boolean (SCM s);

void init_ly_protection ();
unsigned int ly_scm_hash (SCM s);

SCM index_get_cell (SCM cell, Direction d);
SCM index_set_cell (SCM cell, Direction d, SCM val);

SCM ly_snoc (SCM s, SCM list);
SCM ly_split_list (SCM s, SCM lst);
SCM ly_unique (SCM lst);
SCM ly_list_qsort_uniq_x (SCM lst);

SCM ly_output_formats ();
SCM ly_kpathsea_find_file (SCM);

/*
  snarfing.
*/
void add_scm_init_func (void (*) ());

extern "C" {
  typedef SCM (*Scheme_function_unknown) ();
}

#if __GNUC__ > 2 || __GNUC_MINOR__ >= 96
typedef SCM (*Scheme_function_0) ();
typedef SCM (*Scheme_function_1) (SCM);
typedef SCM (*Scheme_function_2) (SCM, SCM);
typedef SCM (*Scheme_function_3) (SCM, SCM, SCM);
#else
typedef SCM (*Scheme_function_0) (...);
typedef SCM (*Scheme_function_1) (...);
typedef SCM (*Scheme_function_2) (...);
typedef SCM (*Scheme_function_3) (...);
#endif

#define scm_cdr ly_cdr
#define scm_car ly_car

#ifndef scm_is_pair
#define scm_is_pair ly_is_pair
#endif

inline SCM ly_car (SCM x) { return SCM_CAR (x); }
inline SCM ly_cdr (SCM x) { return SCM_CDR (x); }
inline bool ly_is_pair (SCM x) { return SCM_I_CONSP (x); }


#endif /* LILY_GUILE_HH */
