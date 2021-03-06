%% Do not edit this file; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.13.10"

\header {
  lsrtags = "expressive-marks, text, tweaks-and-overrides"

%% Translation of GIT committish: b2d4318d6c53df8469dfa4da09b27c15a374d0ca
  texidoces = "
A diferencia de las inscripciones de texto, las lestras de ensayo
no se pueden apilar en un punto concreto de la partitura: sólo se
crea un objeto @code{RehearsalMark}.  Utilizando un compás y línea
divisoria invisibles se puede crear una nueva marca de ensayo,
dando la apariencia de dos marcas en la misma columna.

Este método también puede resultar útil para colocar marcas de
ensayo tanto al final de un sistema como al comienzo del sistema
siguiente.

"
  doctitlees = "Creación de marcas de ensayo simultáneas"

  texidoc = "
Unlike text scripts, rehearsal marks cannot be stacked at a particular
point in a score: only one @code{RehearsalMark} object is created.
Using an invisible measure and bar line, an extra rehearsal mark can be
added, giving the appearance of two marks in the same column. This
method may also prove useful for placing rehearsal marks at both the
end of one system and the start of the following system.

"
  doctitle = "Creating simultaneous rehearsal marks"
} % begin verbatim

{
  \key a \major
  \set Score.markFormatter = #format-mark-box-letters
  \once \override Score.RehearsalMark #'outside-staff-priority = #5000
  \once \override Score.RehearsalMark #'self-alignment-X = #LEFT
  \once \override Score.RehearsalMark #'break-align-symbols = #'(key-signature)
  \mark \markup { \bold { Senza denti } }

  % the hidden measure and bar line
  % \cadenzaOn turns off automatic calculation of bar numbers
  \cadenzaOn
  \once \override Score.TimeSignature #'stencil = ##f
  \time 1/16
  s16 \bar ""
  \cadenzaOff

  \time 4/4
  \once \override Score.RehearsalMark #'self-alignment-X = #LEFT
  \mark \markup { \box \bold Intro }
  d'1
  \mark \default
  d'1
}
