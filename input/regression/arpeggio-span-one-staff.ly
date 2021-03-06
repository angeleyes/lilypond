\header {
  texidoc = "Span arpeggios within one staff also work"
  }

\version "2.12.0"
\layout { ragged-right = ##t }

\new PianoStaff <<
 \set PianoStaff.connectArpeggios = ##t
 \new Staff \relative c'' {
   <<
     {
       c2\arpeggio
     }
     \\
     {
       g2\arpeggio a
     }
   >>
 }
>>
