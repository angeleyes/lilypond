%% Do not edit this file; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.13.4"

\header {
  lsrtags = "pitches, vocal-music"

  texidoc = "
Ambitus indicate pitch ranges for voices.


Accidentals only show up if they are not part of the key signature.
@code{AmbitusNoteHead} grobs also have ledger lines.

"
  doctitle = "Ambitus"
} % begin verbatim

\layout {
  \context {
    \Voice
    \consists "Ambitus_engraver"
  }
}

<<
  \new Staff {
    \relative c' {
      \time 2/4
      c4 f'
    }
  }
  \new Staff {
    \relative c' {
      \time  2/4
      \key d \major
      cis4 as'
    }
  }
>>
