%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.48"

\header {
  lsrtags = "chords"

  texidoc = "
When put together, chord names, a melody, and lyrics form a lead sheet:

"
  doctitle = "Simple lead sheet"
} % begin verbatim
<<
  \chords { c2 g:sus4 f e }
  \relative c'' {
    a4 e c8 e r4
    b2 c4( d)
  }
  \addlyrics { One day this shall be free __ }
>>