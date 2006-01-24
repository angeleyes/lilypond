/*
  file-storage.hh -- declare File_storage, Mapped_file_storage, Simple_file_storage

  source file of the GNU LilyPond music typesetter

  (c) 1997--2006 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#ifndef FILE_STORAGE_HH
#define FILE_STORAGE_HH

#include "flower-proto.hh"

/**
   store a file in-memory.
*/
class File_storage
{
public:
  virtual char const *c_str () const = 0;
  virtual int length () const = 0;
  virtual ~File_storage (){}
};

#endif // FILE_STORAGE_HH
