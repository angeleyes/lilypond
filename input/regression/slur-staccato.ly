
\version "1.9.2"
\header {
texidoc="Manual hack for slur and staccato."
}

\paper { raggedright = ##t}

\score {
  \context Staff \notes\relative c'' {
    \property Voice.Slur \override
      #'attachment-offset = #'((0 . 1) . (0 . 1))
    a-.( g-.  a)-.
    \property Voice.Slur \override
      #'attachment-offset = #'((0 . 1.5) . (0 . 1.5))
    b-.( a-.  b)-.
  }
}	
