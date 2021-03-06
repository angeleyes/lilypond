%% Do not edit this file; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.13.10"

\header {
  lsrtags = "text, vocal-music, spacing"

%% Translation of GIT committish: b2d4318d6c53df8469dfa4da09b27c15a374d0ca
  texidoces = "
Este fragmento de código muestra cómo situar la línea de base de la
letra más cerca del pentagrama.

"
  doctitlees = "Ajuste del especiado vertical de la letra"

%% Translation of GIT committish: 499a511d4166feaada31114e097f86b5e0c56421
  texidocfr = "
Cet extrait illustre la manière de rapprocher la ligne de paroles
de la portée.

"
  doctitlefr = "Ajustement de l'espacement vertical des paroles"


  texidoc = "
This snippet shows how to bring the lyrics line closer to the staff.

"
  doctitle = "Adjusting lyrics vertical spacing"
} % begin verbatim

% Default layout:
<<
  \new Staff \new Voice = melody \relative c' {
    c4 d e f
    g4 f e d
    c1
  }
  \new Lyrics \lyricsto melody { aa aa aa aa aa aa aa aa aa }

% Reducing the minimum space below the staff and above the lyrics:
  \new Staff \with {
    \override VerticalAxisGroup #'minimum-Y-extent = #'(-1 . 4)
  }
  \new Voice = melody \relative c' {
    c4 d e f
    g4 f e d
    c1
  }
  \new Lyrics \with {
    \override VerticalAxisGroup #'minimum-Y-extent = #'(-1.2 . 1)
  }
  \lyricsto melody { aa aa aa aa aa aa aa aa aa }
>>

