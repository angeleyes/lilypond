/*
  keyword.cc -- keywords and identifiers
 */

#include <stdlib.h>

#include "my-lily-lexer.hh"

/* for the keyword table */
struct Keyword_ent
{
  char const *name;
  int     tokcode;
};

struct Keyword_table
{
  Keyword_ent *table;
  int     maxkey;
  Keyword_table (Keyword_ent *);
  int     lookup (char const *s) const;
};


/* for qsort */
int
      tabcmp (void const * p1, void const * p2)
{
  return strcmp (((Keyword_ent const *) p1)->name,
                ((Keyword_ent const *) p2)->name);
}

Keyword_table::Keyword_table (Keyword_ent *tab)
{
  table = tab;

  /* count keywords */
  for (maxkey = 0; table[maxkey].name; maxkey++);

  /* sort them */
  qsort (table, maxkey, sizeof (Keyword_ent), tabcmp);
}

/*
  lookup with binsearch, return tokencode.
*/
int
Keyword_table::lookup (char const *s)const
{
  int     lo,
          hi,
          cmp,
          result;
  lo = 0;
  hi = maxkey;

  /* binary search */
  do
  {
      cmp = (lo + hi) / 2;

      result = strcmp (s, table[cmp].name);

      if (result < 0)
          hi = cmp;
      else
          lo = cmp;
    }
  while (hi - lo > 1);
  if (!strcmp (s, table[lo].name))
  {
      return table[lo].tokcode;
    }
  else
      return -1;              /* not found */
}

