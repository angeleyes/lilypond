%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.38"

\header {
  lsrtags = "chords"
 texidoc = "
The english naming of chords (default) can be changed to german 
(@code{\\germanChords} replaces B and Bes to H and B) or semi-german 
(@code{\\semiGermanChords} replaces B and Bes to H and Bb).



" }
% begin verbatim
music = \chordmode {
  c1/c cis/cis
  b/b bis/bis bes/bes
} 

%% The following is only here to print the names of the
%% chords styles; it can be removed if you do not need to
%% print them.

\layout {
  \context {\ChordNames \consists Instrument_name_engraver }
}

<<
  \new ChordNames {
    \set ChordNames.instrumentName = #"default"
    \music
  }
  \new ChordNames {
    \set ChordNames.instrumentName = #"german"
    \germanChords \music }
  \new ChordNames {
    \set ChordNames.instrumentName = #"semi-german"
    \semiGermanChords \music }
  \context Voice { \music }
>>