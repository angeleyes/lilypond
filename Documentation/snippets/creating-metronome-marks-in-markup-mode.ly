%% Do not edit this file; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.13.4"

\header {
  lsrtags = "staff-notation"

%% Translation of GIT committish: b2d4318d6c53df8469dfa4da09b27c15a374d0ca
  texidoces = "
Se pueden crear indicaciones metronómicas nuevas en modo de
marcado, pero no cambian el tempo en la salida MIDI.

"
  doctitlees = "Crear indicaciones metronómicas en modo de marcado"

%% Translation of GIT committish: d96023d8792c8af202c7cb8508010c0d3648899d
  texidocde = "
Neue Metronombezeichnungen können als Textbeschriftung erstellt werden,
aber sie ändern nicht das Tempo für die MIDI-Ausgabe.

"

  doctitlede = "Eine Metronombezeichnung als Textbeschriftung erstellen"

  texidoc = "
New metronome marks can be created in markup mode, but they will not
change the tempo in MIDI output.

"
  doctitle = "Creating metronome marks in markup mode"
} % begin verbatim

\relative c' {
  \tempo \markup {
    \concat {
      (
      \smaller \general-align #Y #DOWN \note #"16." #1
      " = "
      \smaller \general-align #Y #DOWN \note #"8" #1
      )
    }
  }
  c1
  c4 c' c,2
}