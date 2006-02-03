/*
  keyword.cc -- keywords and identifiers
*/

#include "keyword.hh"

#include <cstring>
#include <cstdlib>
using namespace std;

/* for qsort */
int tabcmp (Keyword_ent const &p1, Keyword_ent const &p2)
{
  return strcmp (p1.name_, p2.name_);
}

Keyword_table::Keyword_table (Keyword_ent *tab)
{
  while (tab->name_)
    table_.push_back (*tab++);

  vector_sort (table_, tabcmp);
}

vsize
Keyword_table::lookup (char const *s) const
{
  Keyword_ent e;
  e.name_ = s;
  vsize idx = binary_search (table_, e, tabcmp);
  if (idx != VPOS)
    return table_[idx].tokcode_;
  return VPOS;
}
