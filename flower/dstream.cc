/*
  dstream.cc -- implement Dstream

  source file of the Flower Library

  (c) 1996, 1997--2000 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/
#include <fstream.h>
#include "dstream.hh"
#include "dictionary-iter.hh"
#include "dictionary.hh"
#include "text-db.hh"
#include "string-convert.hh"
#include "rational.hh"

/// amount of indentation for each level.
const int INDTAB = 2;

/*
  should use Regexp library.
  */
static String
strip_pretty (String pretty_str)
{
  int i = pretty_str.index_i ('(');
  if (i>=0)
    pretty_str = pretty_str.left_str (i);

  int l = pretty_str.index_last_i (' '); // strip until last ' '
  if (l>=0)
    pretty_str = pretty_str.nomid_str (0,l+1);
  return pretty_str;
}

static String
strip_member (String pret)
{
  int l=pret.index_last_i (':')-1;
  if (l>=0)
    pret = pret.left_str (l);
  return pret;
}

Dstream&
Dstream::identify_as (String name)
{
  if (!os_l_)
    return *this;

  String mem (strip_pretty (name));
  String cl (strip_member (mem));
  String idx = cl;

  if (silent_dict_p_->elem_b (mem))
    idx  = mem;
  else if (silent_dict_p_->elem_b (cl))
    idx = cl;
  else
    {
 (*silent_dict_p_)[idx] = default_silence_b_;
    }
  local_silence_b_ = (*silent_dict_p_)[idx];
  if (current_classname_str_ != idx && !local_silence_b_)
    {
      current_classname_str_=idx;
      if (! (*silent_dict_p_)["Dstream"])
	*os_l_ << "[" << current_classname_str_ << ":]"; // messy.
    }
  return *this;
}

bool
Dstream::silent_b (String s) const
{
  if (!silent_dict_p_)
    return 0;
  
  if (!silent_dict_p_->elem_b (s))
    return false;
  return (*silent_dict_p_)[s];
}

Dstream &
Dstream::operator<< (void const *v_l)
{
  output (String_convert::pointer_str (v_l));
  return *this;
}

Dstream &
Dstream::operator << (String s)
{
  output (s);
  return *this;
}

Dstream &
Dstream::operator << (const char * s)
{
  output (String (s));
  return *this;
}

Dstream &
Dstream::operator << (char c)
{
  output (to_str (c));
  return *this;
}

Dstream&
Dstream::operator << (Real r)
{
  output (to_str (r));
  return *this;
}
Dstream &
Dstream::operator << (Rational c)
{
  output (c.str ());
  return *this;
}
Dstream &
Dstream::operator << (int i)
{
  output (to_str (i));
  return *this;
}
  
void
Dstream::output (String s)
{
  if (local_silence_b_|| !os_l_)
    return ;

  for (char const *cp = s.ch_C (); *cp; cp++)
    switch (*cp)
      {
      case '{':
      case '[':
      case '(': indent_level_i_ += INDTAB;
	*os_l_ << *cp;
	break;

      case ')':
      case ']':
      case '}':
	indent_level_i_ -= INDTAB;
	*os_l_ << *cp		;

	assert (indent_level_i_>=0) ;
	break;

      case '\n':
	*os_l_ << '\n' << to_str (' ', indent_level_i_) << flush;
	break;
      default:
	*os_l_ << *cp;
	break;
      }
  return ;
}


Dstream::Dstream (ostream *r, char const * cfg_nm)
{
  os_l_ = r;
  silent_dict_p_ = new Dictionary<bool>;
  default_silence_b_ = false;
  indent_level_i_ = 0;
  if (!os_l_)
    return;

  char const * fn =cfg_nm ? cfg_nm : ".dstreamrc";
  {
    ifstream ifs (fn);	// can 't open
    if (!ifs)
      return;
  }

  Text_db cfg (fn);
  while (!cfg.eof_b ()){
    Text_record  r (cfg++);
    if (r.size () != 2)
      {
	r.message (_ ("not enough fields in Dstream init"));
	continue;
      }
 (*silent_dict_p_)[r[0]] = r[1] == "1";
  }

  if ((*silent_dict_p_).elem_b ("Dstream_default_silence"))
    default_silence_b_ = (*silent_dict_p_)["Dstream_default_silence"];
}


Dstream::~Dstream ()
{
  delete silent_dict_p_;
  assert (!indent_level_i_) ;
}

void
Dstream::clear_silence ()
{
  delete silent_dict_p_;
  silent_dict_p_ = 0;
}

