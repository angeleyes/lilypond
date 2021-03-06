%% Do not edit this file; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.13.10"

\header {
  lsrtags = "chords"

%% Translation of GIT committish: b2d4318d6c53df8469dfa4da09b27c15a374d0ca
  texidoces = "
Se puede usar la propiedad @code{chordNameExceptions} para
almacenar una lista de notaciones espaciales para acordes
específicos.

"
  doctitlees = "Excepciones para los nombres de acorde"

%% Translation of GIT committish: d96023d8792c8af202c7cb8508010c0d3648899d
  texidocde = "
Die Eigenschaft @code{chordNameExceptions} kann benutzt werden, um eine
Liste an besonderen Notationen für bestimmte Akkorde zu speichern.

"
  doctitlede = "Akkordsymbolausnahmen"

  texidoc = "
The property @code{chordNameExceptions} can be used to store a list of
special notations for specific chords.

"
  doctitle = "Chord name exceptions"
} % begin verbatim

% modify maj9 and 6(add9)
% Exception music is chords with markups
chExceptionMusic = {
  <c e g b d'>1-\markup { \super "maj9" }
  <c e g a d'>1-\markup { \super "6(add9)" }
}

% Convert music to list and prepend to existing exceptions.
chExceptions = #( append
  ( sequential-music-to-chord-exceptions chExceptionMusic #t)
  ignatzekExceptions)

theMusic = \chordmode {
  g1:maj9 g1:6.9
  \set chordNameExceptions = #chExceptions
  g1:maj9 g1:6.9
}

\layout {
  ragged-right = ##t
}

<< \context ChordNames \theMusic
   \context Voice \theMusic
>>

