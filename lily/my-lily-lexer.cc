/*
  my-lily-lexer.cc -- implement My_lily_lexer

  source file of the GNU LilyPond music typesetter

  (c) 1997 Han-Wen Nienhuys <hanwen@stack.nl>
*/

#include <strstream.h>
#include <ctype.h>
#include "notename-table.hh"
#include "interval.hh"
#include "identifier.hh"
#include "assoc-iter.hh"
#include "parser.hh"
#include "keyword.hh"
#include "assoc.hh"
#include "my-lily-lexer.hh"
#include "debug.hh"
#include "source-file.hh"
#include "parseconstruct.hh"

static Keyword_ent the_key_tab[]={
    {"accepts", ACCEPTS},
    {"alias", ALIAS},
    {"bar", BAR},
    {"cadenza", CADENZA},
    {"clear", CLEAR},
    {"clef", CLEF},
    {"cm", CM_T},
    {"consists", CONSISTS},
    {"contains", CONTAINS},
    {"duration", DURATIONCOMMAND},
    {"absdynamic", ABSDYNAMIC},
    {"group", GROUP},
    {"geometric", GEOMETRIC},
    {"hshift", HSHIFT},
    {"id", ID},
    {"in", IN_T},
    {"init_end", INIT_END},
    {"requestengraver", REQUESTENGRAVER},
    {"lyric", LYRIC},
    {"key", KEY},
    {"melodic" , MELODIC},
    {"melodic_request", MELODIC_REQUEST},
    {"meter", METER},
    {"midi", MIDI},
    {"mm", MM_T},
    {"multi", MULTI},
    {"note", NOTE},
    {"notenames", NOTENAMES},
    {"octave", OCTAVECOMMAND},
    {"output", OUTPUT},
    {"partial", PARTIAL},
    {"paper", PAPER},
    {"plet", PLET},
    {"pt", PT_T},
    {"score", SCORE},
    {"script", SCRIPT},
    {"skip", SKIP},
    {"staff", STAFF},
    {"start", START_T},
    {"stem", STEM},
    {"table", TABLE},
    {"spandynamic", SPANDYNAMIC}, 
    {"symboltables", SYMBOLTABLES},
    {"tempo", TEMPO},
    {"texid", TEXID},
    {"textstyle", TEXTSTYLE},
    {"transpose", TRANSPOSE},
    {"unitspace", UNITSPACE},
    {"width", WIDTH},
    {"version", VERSION},
    {"grouping", GROUPING},
    {0,0}
};

My_lily_lexer::My_lily_lexer()
{
    keytable_p_ = new Keyword_table(the_key_tab);
    identifier_assoc_p_ = new Assoc<String, Identifier*>;
    errorlevel_i_ = 0;
    post_quotes_b_ = false;
    note_tab_p_ = new Notename_table;
}

int
My_lily_lexer::lookup_keyword(String s)
{
    return keytable_p_->lookup(s);
}

Identifier*
My_lily_lexer::lookup_identifier(String s)
{
    if (!identifier_assoc_p_->elt_b(s))
	return 0;
    
    return (*identifier_assoc_p_)[s];
}


void
My_lily_lexer::add_identifier(Identifier*i)
{
    delete lookup_identifier(i->name_str_);
    (*identifier_assoc_p_)[i->name_str_] = i;
}

My_lily_lexer::~My_lily_lexer()
{
    delete keytable_p_;

    for (Assoc_iter<String,Identifier*>
	     ai(*identifier_assoc_p_); ai.ok(); ai++) {
	mtor << "deleting: " << ai.key()<<'\n';
	delete ai.val();
    }
    delete note_tab_p_;
    delete identifier_assoc_p_;
}
void
My_lily_lexer::print_declarations(bool init_b)const
{
    for (Assoc_iter<String,Identifier*> ai(*identifier_assoc_p_); ai.ok(); 
	 ai++) {
	if (ai.val()->init_b_ == init_b)
	    ai.val()->print();
    }
}

void
My_lily_lexer::LexerError(char const *s)
{
    if (include_stack_.empty()) {
	*mlog << "error at EOF" << s << '\n';
    } else {
	errorlevel_i_ |= 1;
 
	Input spot(source_file_l(),here_ch_C());

	spot.error( s );
    }
}

Melodic_req*
My_lily_lexer::lookup_melodic_req_l(String s)
{
    return note_tab_p_->get_l(s);
}

void
My_lily_lexer::add_notename(String s, Melodic_req *p)
{
    note_tab_p_->add(s,p);
}

void
My_lily_lexer::clear_notenames()
{
    delete note_tab_p_;
    note_tab_p_ = new Notename_table;
}
