/*
  lily-parser.hh -- declare Lily_parser

  source file of the GNU LilyPond music typesetter

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#ifndef LILY_PARSER_HH
#define LILY_PARSER_HH

#include "duration.hh"
#include "input.hh"
#include "parray.hh"
#include "pitch.hh"

/**
   State for the parser.  Do not ever add any variables to parse
   musical content here.  We still have to remove default_duration_.

   TODO: interface is too complicated
*/
class Lily_parser
{
  DECLARE_SMOBS (Lily_parser,);
  friend int yyparse (void *);

  Array<Input> define_spots_;

  char const *here_str0 () const;
  Simultaneous_music *get_chord (Pitch tonic,
				 Array<Pitch> *adds, Array<Pitch> *subs,
				 Pitch *inversion, Pitch *bass, Duration d);
  void set_chord_tremolo (int type_i);
  void set_last_duration (Duration const *);
  void set_last_pitch (Pitch const *);

public:
  Lily_lexer *lexer_;
  Sources *sources_;
  Duration default_duration_;
  String output_basename_;

  int fatal_error_;
  int error_level_;
  bool ignore_version_b_;

  Lily_parser (Sources *sources);
  Lily_parser (Lily_parser const &);

  DECLARE_SCHEME_CALLBACK (layout_description, ());

  void do_init_file ();
  void do_yyparse ();
  void parse_file (String init, String name, String out_name);
  void parse_string (String ly_code);
  void parser_error (String);
  void parser_error (Input const &, String);
  void set_yydebug (bool);
};

DECLARE_UNSMOB (Lily_parser, lily_parser);

SCM ly_parse_file (SCM);
SCM ly_parse_string (SCM);
// SCM ly_parser_add_book_and_score (SCM, SCM);
SCM ly_parser_print_book (SCM, SCM);
SCM ly_parser_print_score (SCM, SCM);
SCM ly_parser_bookify (SCM, SCM);
SCM ly_parser_scorify (SCM, SCM);

Output_def *get_layout (Lily_parser *parser);
Output_def *get_midi (Lily_parser *parser);
Output_def *get_paper (Lily_parser *parser);
SCM get_header (Lily_parser *parser);

#endif /* LILY_PARSER_HH */
