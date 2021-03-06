%% Do not edit this file; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.13.10"

\header {
  lsrtags = "pitches, staff-notation, vocal-music"

%% Translation of GIT committish: b2d4318d6c53df8469dfa4da09b27c15a374d0ca
  doctitlees = "Añadir un ámbito por voz"
  texidoces = "
Se puede añadir un ámbito por cada voz. En este caso, el ámbito se
debe desplazar manualmente para evitar colisiones.

"
%% Translation of GIT committish: d96023d8792c8af202c7cb8508010c0d3648899d
texidocde = "
Ambitus können pro Stimme gesetzt werden. In diesem Fall müssen sie
manual verschoben werden, um Zusammenstöße zu verhindern.

"
doctitlede = "Ambitus pro Stimme hinzufügen"
%% Translation of GIT committish: 59968a089729d7400f8ece38d5bc98dbb3656a2b
  texidocfr = "
L'@code{ambitus} peut être individualisé par voix.  Il faut en pareil
cas éviter qu'ils se chevauchent.

"
  doctitlefr = "Un ambitus par voix"


  texidoc = "
Ambitus can be added per voice. In this case, the ambitus must be moved
manually to prevent collisions.

"
  doctitle = "Adding ambitus per voice"
} % begin verbatim

\new Staff <<
  \new Voice \with {
    \consists "Ambitus_engraver"
  } \relative c'' {
    \override Ambitus #'X-offset = #2.0
    \voiceOne
    c4 a d e
    f1
  }
  \new Voice \with {
    \consists "Ambitus_engraver"
  } \relative c' {
    \voiceTwo
    es4 f g as
    b1
  }
>>

