
\header {


  texidoc = "instrument names may be aligned manually by putting the
names in padded boxes with @code{\markup}."


}

\version "2.10.0"

\paper {
  line-width = 15\cm
}


\new StaffGroup \relative
<<
  \new Staff {
    \set Staff.instrumentName = \markup { \hcenter-in #10 "blabla" }
    c1 c1
  } 
  \new Staff {
    \set Staff.instrumentName = \markup { \hcenter-in #10 "blo" }
    c1 c1
  } 
  
>>