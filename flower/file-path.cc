/*
  file-path.cc - implement File_path

  source file of the Flower Library

  (c) 1997--2006 Han-Wen Nienhuys <hanwen@xs4all.nl>
  Jan Nieuwenhuizen <janneke@gnu.org>
*/

#include "file-path.hh"

#include "std-string.hh"

#include <cstdio>
#include <cerrno>
using namespace std;

#include "config.hh"
#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#ifdef __CYGWIN__
#include <sys/cygwin.h>
#endif

#include "file-name.hh"
#include "warn.hh"

#ifndef PATHSEP
#define PATHSEP ':'
#endif

std::vector<std::string>
File_path::directories () const
{
  return dirs_;
}

/*
  TODO: use split_string.
  
 */

#include <algorithm>
void
File_path::parse_path (std::string p)
{
  ssize len;
  while ((len = p.length ()))
    {
      ssize i = p.find (PATHSEP);
      if (i == NPOS)
	i = len;
      append (p.substr (0, i));
      p = p.substr (std::min (len, i + 1));
    }
}

bool
is_file (std::string file_name)
{
#if 0 /* Check if directory. TODO: encapsulate for autoconf */
  struct stat sbuf;
  if (stat (file_name.c_str (), &sbuf) != 0)
    return false;

  if (! (sbuf.st_mode & __S_IFREG))
    return false;
#endif

#if !STAT_MACROS_BROKEN
  struct stat sbuf;
  if (stat (file_name.c_str (), &sbuf) != 0)
    return false;

  return !S_ISDIR (sbuf.st_mode);
#endif

  if (FILE *f = fopen (file_name.c_str (), "r"))
    {
      fclose (f);
      return true;
    }

  return false;
}

bool
is_dir (std::string file_name)
{
#if !STAT_MACROS_BROKEN
  struct stat sbuf;
  if (stat (file_name.c_str (), &sbuf) != 0)
    return false;

  return S_ISDIR (sbuf.st_mode);
#endif

  if (FILE *f = fopen (file_name.c_str (), "r"))
    {
      fclose (f);
      return true;
    }
  return false;
}

/** Find a file.

Check absolute file name, search in the current dir (DUH! FIXME!),
in the construction-arg (what's that?), and in any other appended
directory, in this order.

@return
The file name if found, or empty string if not found. */

std::string
File_path::find (std::string name) const
{
  if (!name.length () || (name == "-"))
    return name;

#ifdef __MINGW32__
  if (name.find ('\\') != NPOS)
    programming_error ("file name not normalized: " + name);
#endif /* __MINGW32__ */

  /* Handle absolute file name.  */
  File_name file_name (name);
  if (file_name.dir_[0] == DIRSEP && is_file (file_name.to_string ()))
    return file_name.to_string ();

  for (vsize i = 0; i < dirs_.size (); i++)
    {
      File_name file_name (name);
      File_name dir = (std::string) dirs_[i];
      file_name.root_ = dir.root_;
      dir.root_ = "";
      if (file_name.dir_.empty ())
	file_name.dir_ = dir.to_string ();
      else if (!dir.to_string ().empty ())
	file_name.dir_ = dir.to_string ()
	  + std::to_string (DIRSEP) + file_name.dir_;
      if (is_file (file_name.to_string ()))
	return file_name.to_string ();
    }
  return "";
}

/*
  Try to find

  file.EXT,

  where EXT is from EXTENSIONS.
*/
std::string
File_path::find (std::string name, char const *extensions[])
{
  if (name.empty () || name == "-")
    return name;
  
  File_name file_name (name);
  std::string orig_ext = file_name.ext_;
  for (int i = 0; extensions[i]; i++)
    {
      file_name.ext_ = orig_ext;
      if (*extensions[i] && !file_name.ext_.empty ())
	file_name.ext_ += ".";
      file_name.ext_ += extensions[i];
      std::string found = find (file_name.to_string ());
      if (!found.empty ())
	return found;
    }
  
  return "";
}

/** Append a directory, return false if failed.  */
bool
File_path::try_append (std::string s)
{
  if (s == "")
    s = ".";
  if (is_dir (s))
    {
      append (s);
      return true;
    }
  return false;
}

std::string
File_path::to_string () const
{
  std::string s;
  for (vsize i = 0; i < dirs_.size (); i++)
    {
      s = s + dirs_[i];
      if (i < dirs_.size () - 1)
	s += std::to_string (PATHSEP);
    }
  return s;
}

void
File_path::append (std::string str)
{
  dirs_.push_back (str);
}

void
File_path::prepend (std::string str)
{
  dirs_.insert (str, 0);
}
