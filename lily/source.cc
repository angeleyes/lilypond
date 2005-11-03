/*
  source.cc -- implement Sources

  source file of the LilyPond music typesetter

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "source.hh"

#include "killing-cons.tcc"
#include "source-file.hh"
#include "file-path.hh"

Sources::Sources ()
{
  sourcefile_list_ = 0;
  path_ = 0;
  is_binary_ = false;
}

void
Sources::set_binary (bool bo)
{
  is_binary_ = bo;
}

void
Sources::set_path (File_path *f)
{
  path_ = f;
}

/**
   open a file

   @param file_string the file to be opened, name might be changed if it
   is found in a search path. UGH!

   @return 0 if no file found
*/
Source_file *
Sources::get_file (String &file_string) //UGH
{
  if ((file_string != "-") && path_)
    {
      String file_string_o = path_->find (file_string);
      if ((file_string_o == "") && (file_string != ""))
	return 0;
      file_string = file_string_o;
    }
  Source_file *f = new Source_file (file_string);
  add (f);
  return f;
}

void
Sources::add (Source_file *sourcefile)
{
  sourcefile_list_ = new Killing_cons<Source_file> (sourcefile, sourcefile_list_);
}

Sources::~Sources ()
{
  delete sourcefile_list_;
}
/**
   search the list for file whose map contains pointer #str0#

   @return 0 if not found.
*/
Source_file *
Sources::get_sourcefile (char const *str0)
{

  for (Cons<Source_file> *i = sourcefile_list_; i; i = i->next_)
    if (i->car_->contains (str0))
      return i->car_;
  return 0;
}

