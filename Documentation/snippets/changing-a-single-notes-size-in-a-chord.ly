%% Do not edit this file; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.13.10"

\header {
  lsrtags = "simultaneous-notes, editorial-annotations, tweaks-and-overrides"

%% Translation of GIT committish: b2d4318d6c53df8469dfa4da09b27c15a374d0ca
  texidoces = "
Se pueden modificar notas individuales de un acorde con la
instrucción @code{\\tweak}, alterando la propiedad
@code{font-size}.

Dentro de un acorde (entre ángulos simples @code{< >}), antes de
la nota que queremos alterar, situamos la instrucción
@code{\\tweak} seguida por @code{#'font-size} y definimos el
tamaño adecuado como @code{#-2} (una cabeza pequeña).

"
  doctitlees = "Modificar el tamaño de una nota suelta de un acorde"

  texidoc = "
Individual note heads in a chord can be modified with the
@code{\\tweak} command inside a chord, by altering the @code{font-size}
property.


Inside the chord (within the brackets @code{< >}), before the note to
be altered, place the @code{\\tweak} command, followed by
@code{#'font-size} and define the proper size like @code{#-2} (a tiny
note head).



"
  doctitle = "Changing a single note's size in a chord"
} % begin verbatim

\relative c' {
  <\tweak #'font-size #+2 c e g c \tweak #'font-size #-2 e>1^\markup { A tiny e }_\markup { A big c }
}

