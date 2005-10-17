
\header {

  texidoc = "Quotes may contain grace notes. The grace note leading up
  to an unquoted note is not quoted."

}
\paper { raggedright= ##t }

\version "2.7.13"
quoted = \relative c'' {
  R1
  \grace g16 f4 \grace a16 bes4 \grace b16 c4 c4
}

\addquote quoted \quoted


<<
  \new Staff {
    \set Staff.instrument = "quoted"
    \quoted
    }
  \new Staff \new Voice \relative c'' {
    \set Staff.instrument = "quoted"
    R1
    \cueDuring #"quoted" #1  { \grace s16. r2 }
    c2^"original"
  }
>>
