\version "2.7.39"

\header  {
texidoc = "Arpeggio stays clear of accidentals and flipped note heads."
}
\layout {
  ragged-right = ##t
  \context{
    \Staff
    connectArpeggios = ##t
  }
}



\transpose c c' {
  <fis'' g d a>\arpeggio
  <fis, g d a>\arpeggio
  <fis'' g d a>\arpeggio
}
