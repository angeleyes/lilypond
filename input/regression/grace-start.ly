#(ly:set-option 'old-relative)
\version "1.9.0"
\header {
texidoc = "Pieces may begin with grace notes."
}
\score  {\notes \relative c' \context Staff  { 
 \grace {  a'16-[ f]  } g1
 \bar "||"  % test if | and || are synced.
 \grace {  a16-[ bes]  }  c1
 \bar "||"  % test if | and || are synced. 
  }
  \paper { raggedright = ##t}
}

