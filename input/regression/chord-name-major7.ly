\version "2.1.30"
\header {
    texidoc = "The layout of the major 7 can be tuned with
@code{majorSevenSymbol}."
}

\score {\context ChordNames \chords {
      c:7+
      \set majorSevenSymbol = \markup { "j7" }
      c:7+ }
}
