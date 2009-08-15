\version "2.12.0"
\include "example-header.ily"

\include "predefined-guitar-fretboards.ly"

#(set-global-staff-size 17)

global = {
  \time 4/4
  \key g \major
  \partial 4
  \numericTimeSignature
}

melody = \relative c' {
  \global
  d4
  g4 b8( a) g4 fis
  e e e e
  a c8( b) a4 g
  fis a d
}

harmonies = \chordmode {
  \global 
  s4 g1 | c | a:m | d   % 1-3
}

text = \lyricmode {
  My eyes are dim, I can -- not see,
  I have not brought my specs with me!
}

\score {
  <<
    \new ChordNames { \harmonies }
    \new FretBoards { \harmonies }
    \new Staff  { 
      \context Voice = "vocal" { \melody }
    }
    \new Lyrics \lyricsto "vocal" \text
  >>
}

