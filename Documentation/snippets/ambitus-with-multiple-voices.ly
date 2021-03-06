%% Do not edit this file; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.13.10"

\header {
  lsrtags = "pitches, vocal-music"

%% Translation of GIT committish: b2d4318d6c53df8469dfa4da09b27c15a374d0ca
  doctitlees = "Ámbitos con varias voces"
  texidoces = "
La adición del grabador @code{Ambitus_engraver} al contexto de
@code{Staff} crea un solo ámbito por pentagrama, incluso en el caso de
pentagramas con varias voces.

"

%% Translation of GIT committish: d96023d8792c8af202c7cb8508010c0d3648899d
  texidocde = "
Indem man den @code{Ambitus_engraver} im @code{Staff}-Kontext
hinzufügt, erhält man einen einzigen Ambitus pro System, auch in dem
Fall, dass mehrere Stimmen sich im gleichen System befinden.
"
  doctitlede = "Ambitus mit vielen Stimmen"
%% Translation of GIT committish: 59968a089729d7400f8ece38d5bc98dbb3656a2b
  texidocfr = "
Si plusieurs voix se trouvent sur une même portée, on peut attribuer le
graveur @code{Ambitus_engraver} au contexte @code{Staff} afin d'obtenir
l'ambitus de toutes les voix cumulées, non d'une seule des voix actives. 

"
  doctitlefr = "Ambitus sur plusieurs voix"


  texidoc = "
Adding the @code{Ambitus_engraver} to the @code{Staff} context creates
a single ambitus per staff, even in the case of staves with multiple
voices.

"
  doctitle = "Ambitus with multiple voices"
} % begin verbatim

\new Staff \with {
  \consists "Ambitus_engraver"
  }
<<
  \new Voice \relative c'' {
    \voiceOne
    c4 a d e
    f1
  }
  \new Voice \relative c' {
    \voiceTwo
    es4 f g as
    b1
  }
>>


