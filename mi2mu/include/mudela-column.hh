//
// mudela-column.hh -- declare Mudela_column
//
// copyright 1997 Jan Nieuwenhuizen <jan@digicash.com>

#ifndef MUDELA_COLUMN_HH
#define MUDELA_COLUMN_HH

#include "proto.hh"
#include "mi2mu-proto.hh"
#include "moment.hh"
#include "plist.hh"

/// (mudela_column)
class Mudela_column 
{
public:
    Mudela_column (Mudela_score* mudela_score_l, Moment mom);

    void add_item (Mudela_item* mudela_item_l);
    Moment at_mom();

    Link_list<Mudela_item*> mudela_item_l_list_;
    Moment at_mom_;
    Mudela_score* mudela_score_l_;
};

#endif // MUDELA_COLUMN_HH

