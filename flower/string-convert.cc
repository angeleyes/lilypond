/*
  PROJECT: FlowerSoft C++ library
  FILE   : string-convert.cc

  --*/

#include "string-convert.hh"

#include <cstring>
#include <cstdio>
using namespace std;

#include "libc-extension.hh"
#include "rational.hh"
#include "std-vector.hh"

/**
   A safe length for stringconversion buffers.

   worst case would be %f printing HUGE (or 1/HUGE), which is approx
   2e318, this number would have approx 318 zero's in its string.

   Should enlarge buff dynamically.

   @see
   man 3 snprintf
*/
static const int STRING_BUFFER_LEN = 1024;

std::string
String_convert::bool_string (bool b)
{
  return std::string (b ? "true" : "false");
}

std::string
String_convert::bin2hex (std::string bin_string)
{
  std::string str;
  Byte const *byte = (Byte const*)bin_string.data ();
  for (ssize i = 0; i < bin_string.length (); i++)
    {
      str += to_string ((char)nibble2hex_byte (*byte >> 4));
      str += to_string ((char)nibble2hex_byte (*byte++));
    }
  return str;
}

int
String_convert::bin2int (std::string bin_string)
{
  return bin2unsigned (bin_string);
}

unsigned
String_convert::bin2unsigned (std::string bin_string)
{
  assert (bin_string.length () <= (int)sizeof (unsigned));

  unsigned result_u = 0;
  for (ssize i = 0; i < bin_string.length (); i++)
    {
      result_u <<= 8;
      result_u += (Byte)bin_string[ i ];
    }
  return result_u;
}

// breendet imp from std::string
int
String_convert::dec2int (std::string dec_string)
{
  if (!dec_string.length ())
    return 0;

  long l = 0;
  int conv = sscanf (dec_string.c_str (), "%ld", &l);
  assert (conv);

  return (int)l;
}

std::string
String_convert::i64_string (I64 i64, char const *fmt)
{
  char buffer[STRING_BUFFER_LEN];
  snprintf (buffer, STRING_BUFFER_LEN,
	    (fmt ? fmt : "%Ld"), i64); // assume radix 10
  return std::string (buffer);
}
// breendet imp from std::string
double
String_convert::dec2double (std::string dec_string)
{
  if (!dec_string.length ())
    return 0;
  double d = 0;
  int conv = sscanf (dec_string.c_str (), "%lf", &d);
  assert (conv);
  return d;
}

int
String_convert::hex2bin (std::string hex_string, std::string &bin_string_r)
{
  if (hex_string.length () % 2)
    hex_string = "0" + hex_string;

  bin_string_r = "";
  Byte const *byte = (Byte const*) hex_string.data ();
  ssize i = 0;
  while (i < hex_string.length ())
    {
      int high_i = hex2nibble (*byte++);
      int low_i = hex2nibble (*byte++);
      if (high_i < 0 || low_i < 0)
	return 1; // illegal char
      bin_string_r += to_string ((char) (high_i << 4 | low_i), 1);
      i += 2;
    }
  return 0;
}

std::string
String_convert::hex2bin (std::string hex_string)
{
  std::string str;
  //  silly, asserts should alway be "on"!
  //    assert (!hex2bin (hex_string, str) );
  int error_i = hex2bin (hex_string, str);
  assert (!error_i);
  return str;
}

int
String_convert::hex2nibble (Byte byte)
{
  if (byte >= '0' && byte <= '9')
    return byte - '0';
  if (byte >= 'A' && byte <= 'F')
    return byte - 'A' + 10;
  if (byte >= 'a' && byte <= 'f')
    return byte - 'a' + 10;
  return -1;
}

// stupido.  Should use int_string ()
std::string
String_convert::int2dec (int i, int length_i, char ch)
{
  char fill_char = ch;
  if (fill_char)
    fill_char = '0';

  // ugh
  std::string dec_string = to_string (i);

  // ugh
  return to_string (fill_char, length_i - dec_string.length ()) + dec_string;
}

// stupido.  Should use int_string ()
std::string
String_convert::unsigned2hex (unsigned u, ssize length, char fill_char)
{
  std::string str;
  if (!u)
    str = "0";

#if 1 // both go...
  while (u)
    {
      str = to_string ((char) ((u % 16)["0123456789abcdef"])) + str;
      u /= 16;
    }
#else
  str += int_string (u, "%x");	// hmm. %lx vs. %x -> portability?
#endif

  str = to_string (fill_char, length - str.length ()) + str;
  while ((str.length () > length) && (str[ 0 ] == 'f'))
    str = str.substr (2);

  return str;
}

std::string
String_convert::int2hex (int i, int length_i, char fill_char)
{
  return unsigned2hex ((unsigned)i, length_i, fill_char);
}

Byte
String_convert::nibble2hex_byte (Byte byte)
{
  if ((byte & 0x0f) <= 9)
    return (byte & 0x0f) + '0';
  else
    return (byte & 0x0f) - 10 + 'a';
}
/**
   Convert an integer to a string

   @param
   #fmt# is a printf style format, default assumes "%d" as format.
*/
std::string
String_convert::int_string (int i, char const *fmt)
{
  char buffer[STRING_BUFFER_LEN];
  snprintf (buffer, STRING_BUFFER_LEN,
	    (fmt ? fmt : "%d"), i); // assume radix 10
  return std::string (buffer);
}

std::string
String_convert::form_string (char const *format, ...)
{
  va_list args;
  va_start (args, format);
  char buffer[STRING_BUFFER_LEN];
  vsnprintf (buffer, STRING_BUFFER_LEN, format, args);
  va_end (args);
  return std::string (buffer);
}

std::string
String_convert::vform_string (char const *format, va_list args)
{
  char buffer[STRING_BUFFER_LEN];
  vsnprintf (buffer, STRING_BUFFER_LEN, format, args);
  return std::string (buffer);
}

/**
   Convert a double to a string.

   @param #fmt# is a printf style format, default assumes "%lf" as format
*/
std::string
String_convert::double_string (double f, char const *fmt)
{
  char buf[STRING_BUFFER_LEN];

  snprintf (buf, STRING_BUFFER_LEN, fmt ? fmt : "%f", f);
  return std::string (buf);
}

/**
   Make a string from a single character.

   @param
   #n# is a repetition count, default value is 1
*/
std::string
String_convert::char_string (char c, int n)
{
  n = n >= 0 ? n : 0;
  char *ch = new char[ n ];
  memset (ch, c, n);
#if STD_STRING
  std::string s (ch, n);
#else
  std::string s (ch, n);
#endif
  delete[] ch;
  return s;
}

std::string
String_convert::rational_string (Rational r)
{
  return r.to_string ();
}

std::string
String_convert::pointer_string (void const *l)
{
  char buffer[STRING_BUFFER_LEN];
  snprintf (buffer, STRING_BUFFER_LEN, "%p", l); // assume radix 10
  return std::string (buffer);
}

/**
   Convert a double to a string.

   @param
   #n# is the number of nonzero digits
*/
std::string
String_convert::precision_string (double x, int n)
{
  std::string format = "%." + to_string (max (0, n - 1)) + "e";
  std::string str = double_string (abs (x), format.c_str ());

  int exp = dec2int (str.substr (str.length () - 3));
  str = str.substr (0, str.length () - 4);

  while (str[str.length () - 1] == '0')
    str = str.substr (0, str.length () - 1);
  if (str[str.length () - 1] == '.')
    str = str.substr (0, str.length () - 1);

  if (exp == 0)
    return (sign (x) > 0 ? str : "-" + str);

  str = str.substr (0, 1) + str.substr (2);
  ssize dot = 1 + exp;
  if (dot <= 0)
    str = "0." + to_string ('0', -dot) + str;
  else if (dot >= str.length ())
    str += to_string ('0', dot - str.length ());
  else if ((dot > 0) && (dot < str.length ()))
    str = str.substr (0, dot) + "." + str.substr (dot);
  else
    assert (0);

  return (sign (x) > 0 ? str : "-" + str);
}

std::vector<std::string>
String_convert::split (std::string str, char c)
{
  std::vector<std::string> a;
  ssize i = str.find (c);
  while (i != NPOS)
    {
      std::string s = str.substr (0, i);
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

std::string
String_convert::long_string (long l)
{
  char s[STRING_BUFFER_LEN];
  sprintf (s, "%ld", l);
  return s;
}

std::string
String_convert::unsigned_string (unsigned u)
{
  char s[STRING_BUFFER_LEN];
  sprintf (s, "%ud", u);
  return s;
}

std::string
String_convert::pad_to (std::string s, int n)
{
  return s + std::string (max (int(n - s.length ()), 0), ' ');
}

std::string
String_convert::to_upper (std::string s)
{
  return strnupr ((char *)s.c_str (), s.length ());
}

std::string
String_convert::to_lower (std::string s)
{
  return strnlwr ((char *)s.c_str (), s.length ());
}

std::string
String_convert::reverse (std::string s)
{
  return (char*) memrev ((unsigned char *)s.data (), s.length ());
}
