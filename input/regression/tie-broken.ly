\header
{
texidoc = "Ties behave properly at line breaks."
  }

\version "2.7.13"

\paper { raggedright = ##t } 

\relative c'
{
  r2...  <b e a>16 ~
  \break
  <b e a>4 r4 r2
}

