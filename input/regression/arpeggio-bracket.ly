

\version "2.6.0"
\header{
  texidoc="
A square bracket on the left indicates that the player should not
arpeggiate the chord.
"
}

\relative c''{
  \arpeggioBracket
  
  <fis,  d a >\arpeggio
}

\layout { raggedright= ##t }

