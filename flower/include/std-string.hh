/*
  std-string.hh -- declare string

  source file of the GNU LilyPond music typesetter

  (c) 2006 Jan Nieuwenhuizen <janneke@gnu.org>
*/

#ifndef STD_STRING_HH
#define STD_STRING_HH
 
#include "compare.hh"

#include <string>

using namespace std;

typedef size_t ssize;
#define NPOS string::npos

string to_string (string s);
string to_string (char c, int n=1);
string to_string (int i, char const *format=0);
string to_string (double f, char const *format=0);
string to_string (long);
string to_string (long unsigned);
string to_string (unsigned);
string to_string (bool b);
string to_string (char const *format, ...)
  __attribute__ ((format (printf, 1, 2)));
  
string &replace_all (string &str, string find, string replace);
string &replace_all (string &str, char find, char replace);
char *string_copy (string s);

int string_compare (string const &, string const &);

INSTANTIATE_COMPARE (string const &, string_compare);


#endif /* STD_STRING_HH */
