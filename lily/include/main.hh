/*
  main.hh -- declare global entry points

  source file of the GNU LilyPond music typesetter

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/
#ifndef MAIN_HH
#define MAIN_HH

#include "lily-proto.hh"
#include "array.hh"

void debug_init ();
void set_debug (bool);
void do_scores ();
void clear_scores ();
void add_score (Score* s);
void set_default_output (String s);
String find_file (String);
void call_constructors ();
Array<String> get_inclusion_names ();
void set_inclusion_names (Array<String>);


extern String init_name_global;

/* options */
extern Array<String> dump_header_fieldnames_global;
extern String output_backend_global;
extern String output_name_global;
extern bool be_safe_global;
extern bool be_verbose_global;
extern bool store_locations_global;
extern bool do_internal_type_checking_global;
extern bool is_pango_format_global;
extern char const *prefix_directories[];

/*
  todo: collect in Output_option struct? 
 */
extern String output_format_global;

extern bool make_preview;
extern bool make_pages;

/* misc */
extern Array<String> failed_files;
extern int exit_status_global;
extern File_path global_path;

/*
  Debugging options
 */

#ifndef NDEBUG
#define DEBUG_SLUR_SCORING 1
#endif

#endif /* MAIN_HH */
