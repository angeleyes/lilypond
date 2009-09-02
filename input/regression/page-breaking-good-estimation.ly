\version "2.13.4"

\header {
  texidoc = "The page breaking algorithm can handle clefs combined
with lyrics.  That is, the Y-extent approximations are a little more
accurate than just using bounding boxes.  In particular, everything
should fit on one page here."
}

#(set-default-paper-size "a6")

Melody = \relative c' {
  c2 c | c c | c c | \break
  c2 c | c c | c c
}
Words = \lyricmode{
  bla bla bla bla bla bla
  bla bla bla bla bla bla
}
\book {
  \score {
    <<
      \new Staff{\Melody}
      \addlyrics{\Words}
      \new Staff{\Melody}
      \addlyrics{\Words}
      \new Staff{\Melody}
      \addlyrics{\Words}
      \new Staff{\Melody}
      \addlyrics{\Words}
    >>
  }
}
