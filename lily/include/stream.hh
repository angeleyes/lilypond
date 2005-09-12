/*
  stream.hh -- declare compatibility glue for gcc 3.

  source file of the GNU LilyPond music typesetter

  (c) 2001--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#ifndef STREAM_HH
#define STREAM_HH

#include <iostream>
#include <sstream>
using namespace std;

#include "string.hh"

#if __GNUC__ > 2
ostream *open_file_stream (String file_name,
				ios_base::openmode mode = ios::out);
#else
ostream *open_file_stream (String file_name, int mode = ios::out);
#endif
void close_file_stream (ostream *os);

#endif /* STREAM_HH */

