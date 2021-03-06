%% Do not edit this file; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.13.10"

\header {
  lsrtags = "text, vocal-music, contexts-and-engravers, template"

%% Translation of GIT committish: b2d4318d6c53df8469dfa4da09b27c15a374d0ca
  texidoces = "
Esta plantilla es, básicamente, la misma que la sencilla plantilla
\"Conjunto vocal\", excepto que aquí todas las líneas de letra se
colocan utilizando @code{alignAboveContext} y
@code{alignBelowContext}.

"
  doctitlees = "Plantilla para conjunto vocal con letras alineadas encima y debajo de los pentagramas"

%% Translation of GIT committish: 06d99c3c9ad1c3472277b4eafd7761c4aadb84ae
  texidocja = "
このテンプレートは基本的に単純な \"合唱\" テンプレートと同じですが、歌詞が
@code{alignAboveContext} と @code{alignBelowContext} を用いて配置されています。
"
%% Translation of GIT committish: d96023d8792c8af202c7cb8508010c0d3648899d
  texidocde = "
In diesem Beispiel werden die Texte mit den Befehlen
@code{alignAboveContext} und @code{alignBelowContext}
über und unter dem System angeordnet.
"

  doctitlede = "Vorlage für Vokalensemble mit dem Gesangstext über und unter dem System"

%% Translation of GIT committish: 892286cbfdbe89420b8181975032ea975e79d2f5
  texidocfr = "
Ce canevas ressemble beaucoup à celui pour chœur à quatre voix mixtes.
La différence réside dans le fait que les paroles sont positionnées en
ayant recours à @code{alignAboveContext} et @code{alignBelowContext}.

"
  doctitlefr = "Ensemble vocal avec alignement des paroles selon le contexte"

  texidoc = "
This template is basically the same as the simple @qq{Vocal ensemble}
template, with the exception that here all the lyrics lines are placed
using @code{alignAboveContext} and @code{alignBelowContext}.

"
  doctitle = "Vocal ensemble template with lyrics aligned below and above the staves"
} % begin verbatim

global = {
  \key c \major
  \time 4/4
}

sopMusic = \relative c'' {
  c4 c c8[( b)] c4
}
sopWords = \lyricmode {
  hi hi hi hi
}

altoMusic = \relative c' {
  e4 f d e
}
altoWords = \lyricmode {
  ha ha ha ha
}

tenorMusic = \relative c' {
  g4 a f g
}
tenorWords = \lyricmode {
  hu hu hu hu
}

bassMusic = \relative c {
  c4 c g c
}
bassWords = \lyricmode {
  ho ho ho ho
}

\score {
  \new ChoirStaff <<
    \new Staff = women <<
      \new Voice = "sopranos" { \voiceOne << \global \sopMusic >> }
      \new Voice = "altos" { \voiceTwo << \global \altoMusic >> }
    >>
    \new Lyrics \with { alignAboveContext = women } \lyricsto sopranos \sopWords
    \new Lyrics \with { alignBelowContext = women } \lyricsto altos \altoWords
    % we could remove the line about this with the line below, since we want
    % the alto lyrics to be below the alto Voice anyway.
    % \new Lyrics \lyricsto altos \altoWords

    \new Staff = men <<
      \clef bass
      \new Voice = "tenors" { \voiceOne << \global \tenorMusic >> }
      \new Voice = "basses" { \voiceTwo << \global \bassMusic >> }
    >>
    \new Lyrics \with { alignAboveContext = men } \lyricsto tenors \tenorWords
    \new Lyrics \with { alignBelowContext = men } \lyricsto basses \bassWords
    % again, we could replace the line above this with the line below.
    % \new Lyrics \lyricsto basses \bassWords
  >>
  \layout {
    \context {
      % a little smaller so lyrics
      % can be closer to the staff
      \Staff
      \override VerticalAxisGroup #'minimum-Y-extent = #'(-3 . 3)
    }
  }
}

