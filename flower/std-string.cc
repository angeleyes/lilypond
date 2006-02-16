/*
  std-string.cc -- implement external interface for Std_String

  source file of the GNU LilyPond music typesetter

  (c) 2006  Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include "std-string.hh"
#include "string-convert.hh"

string
to_string (string s)
{
  return s;
}

string
to_string (char c, int n)
{
  return string (max (n, 0), c);
}

string
to_string (double f, char const *format)
{
  return String_convert::double_string (f, format);
}

string
to_string (int i, char const *format)
{
  return String_convert::int_string (i, format);
}

string
to_string (bool b)
{
  return String_convert::bool_string (b);
}

string
to_string (long b)
{
  return String_convert::long_string (b);
}

string
to_string (long unsigned b)
{
  return String_convert::unsigned_string (b);
}

string
to_string (unsigned u)
{
  return String_convert::unsigned_string (u);
}

string
to_string (char const *format, ...)
{
  va_list args;
  va_start (args, format);
  string str = String_convert::vform_string (format, args);
  va_end (args);
  return str;
}

string &
replace_all (string &str, string find, string replace)
{
  ssize len = find.length ();
  for (ssize i = str.find (find); i != NPOS; i = str.find (find, i + len))
    str = str.replace (i, len, replace);
  return str;
}

string &
replace_all (string &str, char find, char replace)
{
  for (ssize i = str.find (find); i != NPOS; i = str.find (find, i + 1))
    str[i] = replace;
  return str;
}

char *
string_copy (string s)
{
  ssize len = s.length ();
  char *dest = new char[len + 1];
  //s.copy (dest, len + 1);
  memcpy (dest, s.c_str (), len + 1);
  return dest;
}

int
string_compare (string const &a, string const &b)
{
  return a.compare (b);
}

#include "std-vector.hh"
vector<string>
string_split (string str, char c)
{
  vector<string> a;
  ssize i = str.find (c);
  while (i != NPOS)
    {
      string s = str.substr (0, i);
      a.push_back (s);
      while (str[++i] == c)
	;
      str = str.substr (i);
      i = str.find (c);
    }
  if (str.length ())
    a.push_back (str);
  return a;
}
