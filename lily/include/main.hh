/*
  main.hh -- declare global entry points

  source file of the GNU LilyPond music typesetter

  (c)  1997--1999 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/
#ifndef MAIN_HH
#define MAIN_HH

#include "lily-proto.hh"

void debug_init();
void set_debug (bool);
void do_scores();
void clear_scores();
void add_score (Score* s);
void set_default_output (String s);
String find_file (String);
void call_constructors ();

extern Sources* source_global_l;
extern bool no_paper_global_b;
extern bool safe_global_b;
extern bool no_timestamps_global_b;
extern bool find_old_relative_b;

extern int exit_status_i_;
extern bool experimental_features_global_b;
extern char const* output_global_ch;
extern bool dependency_global_b;
extern bool version_ignore_global_b;


extern Array<String> get_inclusion_names ();
extern void set_inclusion_names (Array<String>);

extern File_path global_path;

extern String default_outname_base_global;
extern String default_outname_suffix_global;
extern int default_count_global;
extern All_font_metrics *all_fonts_global_p;


#endif
