/*
  memory-stream.cc --  implement Memory_out_stream

  source file of the GNU LilyPond music typesetter

  (c) 2005 Han-Wen Nienhuys <hanwen@xs4all.nl>

*/

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "memory-stream.hh"


/*
  TODO: add read support as well.
*/
const int Memory_out_stream::block_size_ = 1024;

cookie_io_functions_t
Memory_out_stream::functions_ = {
  Memory_out_stream::reader,
  Memory_out_stream::writer,
  Memory_out_stream::seeker,
  Memory_out_stream::cleaner
};

int
Memory_out_stream::cleaner (void *cookie)
{
  Memory_out_stream *stream = (Memory_out_stream*) cookie;

  stream->file_ = 0;
  return 0;
}

Memory_out_stream::Memory_out_stream ()
{
  size_ = 0;
  buffer_ = 0;
  buffer_blocks_ = 0;
  file_ = fopencookie ((void*) this, "w", functions_);
}

Memory_out_stream::~Memory_out_stream ()
{
  if (file_)
    fclose (file_);

  free (buffer_);
}

FILE *
Memory_out_stream::get_file () const
{
  return file_;
}

ssize_t
Memory_out_stream::get_length () const
{
  return size_;
}

const char *
Memory_out_stream::get_string () const
{
  return buffer_;
}

ssize_t
Memory_out_stream::writer (void *cookie,
			   const char *buffer,
			   size_t size)
{
  Memory_out_stream *stream = (Memory_out_stream*) cookie;

  ssize_t newsize = stream->size_ + size;

  bool change = false;
  while (newsize > stream->buffer_blocks_ * block_size_)
    {
      stream->buffer_blocks_ *= 2;
      stream->buffer_blocks_ += 1;
      change = true;
    }

  if (change)
    stream->buffer_ = (char*) realloc (stream->buffer_,
				       stream->buffer_blocks_ * block_size_);

  memcpy (stream->buffer_ + stream->size_, buffer, size);
  stream->size_ = newsize;

  return size;
}

ssize_t
Memory_out_stream::reader (void *cookie,
			   char *buffer,
			   size_t size)
{
  (void) cookie;
  (void) buffer;
  (void) size;

  assert (false);
  return 0;
}

int
Memory_out_stream::seeker (void *, off64_t *, int whence)
{
  (void) whence;

  assert (false);
  return 0;
}
