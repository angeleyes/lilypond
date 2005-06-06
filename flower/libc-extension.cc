/*
  libc-extension.cc --  compensate for lacking libc functions.

  source file of the flowerlib

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include <cmath>
#include <cstdio>
#include <cstring>
#include <cctype>

#include "libc-extension.hh"

char *
strnlwr (char *start, int n)
{
  char *p = start + n;
  while (--p >= start)
    {
      *p = tolower (*p);    /* a macro on some compilers */
    }
  return start;
}

char *
strnupr (char *start, int n)
{
  char *p = start + n;
  while (--p >= start)
    {
      *p = toupper (*p);    /* a macro on some compilers */
    }
  return start;
}

/*
  There are some strange problems with round() on early glibcs.
*/
double
my_round (double x)
{
  return floor (x -0.5)+ 1.0;
}

#ifndef isinf
#if !HAVE_ISINF
int
isinf (double x)
{
  return x && (x == x/ 2);
}
#endif
#endif

#if !HAVE_MEMMEM

/** locate a substring. #memmem# finds the first occurrence of
    #needle# in #haystack#.  This is not ANSI-C.

    The prototype is not in accordance with the Linux Programmer's
    Manual v1.15, but it is with /usr/include/string.h   */

unsigned char *
_memmem (unsigned char const *haystack, int haystack_len,
	 unsigned char const *needle, int needle_len)
{
  unsigned char const *end_haystack = haystack + haystack_len - needle_len + 1;
  unsigned char const *end_needle = needle + needle_len;

  /* Ahhh ... Some minimal lowlevel stuff. This *is* nice; Varation
     is the spice of life */
  while (haystack < end_haystack)
    {
      unsigned char const *subneedle = needle;
      unsigned char const *subhaystack = haystack;
      while (subneedle < end_needle)
	if (*subneedle++ != *subhaystack++)
	  goto next;

      /* Completed the needle.  Gotcha.  */
      return (unsigned char *) haystack;
    next:
      haystack++;
    }
  return 0;
}

void *
memmem (void const *haystack, int haystack_len,
	void const *needle, int needle_len)
{
  unsigned char const *haystack_byte_c = (unsigned char const *)haystack;
  unsigned char const *needle_byte_c = (unsigned char const *)needle;
  return _memmem (haystack_byte_c, haystack_len, needle_byte_c, needle_len);
}

#endif

unsigned char *
memrchr (unsigned char const *p, int n, char c)
{
  const unsigned char *q = p + n;
  while (q > p)
    {
      if (*--q == c)
	return (unsigned char *)q;
    }
  return 0;
}

template<class T>
inline void
my_swap (T &t1, T &t2, T &tmp)
{
  tmp = t1;
  t1 = t2;
  t2 = tmp;
}

unsigned char *
memrev (unsigned char *byte, int length)
{
  unsigned char tmp_byte;
  unsigned char *left = byte;
  unsigned char *right = byte + length;

  while (right > left)
    my_swap (*right--, *left++, tmp_byte);
  return byte;
}

#if ! HAVE_SNPRINTF
int
snprintf (char *str, size_t n, char const *format, ...)
{
  va_list ap;
  va_start (ap, format);
  int i = vsprintf (str, format, ap);
  if (i > 0 && (unsigned) i > n)
    assert (false);
  va_end (ap);
  return i;
}
#endif

#if ! HAVE_VSNPRINTF
int
vsnprintf (char *str, size_t n, char const *format, va_list args)
{
  int i = vsprintf (str, format, args);
  if (i > 0 && (unsigned) i > n)
    assert (false);
  return i;
}
#endif

