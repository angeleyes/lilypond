/*
  file-storage.cc -- implement Mapped_file_storage

  source file of the GNU LilyPond music typesetter

  (c) 1997 Han-Wen Nienhuys <hanwen@stack.nl>
  Jan Nieuwenhuizen <jan@digicash.com>
*/
#include <sys/types.h>		// open, mmap
#include <sys/stat.h>		// open
#include <sys/mman.h>		// mmap
#include <limits.h>		// INT_MAX
#include <fcntl.h>		// open 
#include <unistd.h>		// close, stat
#include <stdio.h>		// fdopen
#include <string.h>		// strerror
#include <errno.h>		// errno



#include "string.hh"
#include "proto.hh"
#include "warn.hh"
#include "file-storage.hh"

Mapped_file_storage::Mapped_file_storage(String s)
{
    data_caddr_ = 0;
    fildes_i_ = 0;
    size_off_ = 0;
    open(s);
}

char const*
Mapped_file_storage::ch_C()const
{
    return (char const*)data_caddr_;
}

void
Mapped_file_storage::map()
{
    if ( fildes_i_ == -1 )
	return;

    data_caddr_ = (caddr_t)mmap( (void*)0, size_off_, PROT_READ, MAP_SHARED, fildes_i_, 0 );

    if ( (int)data_caddr_ == -1 )
	warning( String( "can't map: error no: " ) + strerror( errno ));
}


void
Mapped_file_storage::open(String name_str)
{
    fildes_i_ = ::open( name_str, O_RDONLY );	
	    
    if ( fildes_i_ == -1 ) 
      {
	warning( String( "can't open: " ) + name_str + String( ": " ) + strerror( errno )); 
        return;
      }

    struct stat file_stat;
    fstat( fildes_i_, &file_stat );
    size_off_ = file_stat.st_size;
    map();
}

void
Mapped_file_storage::unmap()
{
    if ( data_caddr_ ) 
      {
	munmap( data_caddr_, size_off_ );
    	data_caddr_ = 0;
	size_off_ = 0;
      }
}

void
Mapped_file_storage::close()
{
    unmap();
    if ( fildes_i_ ) 
      {
	::close( fildes_i_ );
	fildes_i_ = 0;
      }
}

int
Mapped_file_storage::length_i()const
{
    return size_off_;
}

Mapped_file_storage::~Mapped_file_storage()
{
    close();
}
