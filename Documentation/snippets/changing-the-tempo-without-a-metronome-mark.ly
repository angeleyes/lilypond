%% Do not edit this file; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.13.10"

\header {
  lsrtags = "staff-notation, midi"

%% Translation of GIT committish: b2d4318d6c53df8469dfa4da09b27c15a374d0ca
  texidoces = "
Para cambiar el tempo en la salida MIDI sin
imprimir nada, hacemos invisible la indicación metronómica:

"

  doctitlees = "Cambiar el tempo sin indicación metronómica"

%% Translation of GIT committish: d96023d8792c8af202c7cb8508010c0d3648899d
  texidocde = "
Um das Tempo für die MIDI-Ausgabe zu ändern, ohne eine Tempoangabe in den
Noten auszugeben, kann die Metronombezeichnung unsichtbar gemacht werden:

"

  doctitlede = "Das Tempo ohne Metronom-Angabe verändern"
%% Translation of GIT committish: d78027a94928ddcdd18fd6534cbe6d719f80b6e6
  texidocfr = "
Vous pouvez indiquer un changement de tempo pour le fichier MIDI sans
pour autant l'imprimer.  Il suffit alors de le rendre invisible aux
interprètes. 

"
  doctitlefr = "Changement de tempo sans indication sur la partition"


  texidoc = "
To change the tempo in MIDI output without printing anything, make the
metronome mark invisible.

"
  doctitle = "Changing the tempo without a metronome mark"
} % begin verbatim

\score {
  \new Staff \relative c' {
    \tempo 4 = 160
    c4 e g b
    c4 b d c
    \set Score.tempoHideNote = ##t
    \tempo 4 = 96
    d,4 fis a cis
    d4 cis e d
  }
  \layout { }
  \midi { }
}
