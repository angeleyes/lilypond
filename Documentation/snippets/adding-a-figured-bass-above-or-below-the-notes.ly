%% Do not edit this file; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.13.10"

\header {
  lsrtags = "chords, ancient-notation, contexts-and-engravers"

%% Translation of GIT committish: b2d4318d6c53df8469dfa4da09b27c15a374d0ca
  texidoces = "
Al escribir un bajo cifrado, existe una forma de especificar si
queremos que las cifras se sitúen encima o debajo de las notas del
bajo, mediante la definición de la propiedad
@code{BassFigureAlignmentPositioning #'direction} (exclusivamente
dentro de un contexto @code{Staff}). Se puede elegir entre
@code{#UP} (o @code{#1}, arriba), @code{#CENTER} (o @code{#0},
centrado) y @code{#DOWN} (o @code{#-1}, abajo).

Como podemos ver, esta propiedad se puede cambiar tantas veces
como queramos.  Utilice @code{\\once \\override} si no quiere que el
truco se aplique a toda la partitura.

"
  doctitlees = "Añadir un bajo cifrado encima o debajo de las notas"

%% Translation of GIT committish: 7eb450e8e6c935410b8f3212cb53e5a731f4f33f
  doctitlefr = "Ajout d'une basse chiffrée au-dessus ou au-dessous des notes"

  texidoc = "
When writing a figured bass, here's a way to specify if you want your
figures to be placed above or below the bass notes, by defining the
@code{BassFigureAlignmentPositioning #'direction} property (exclusively
in a @code{Staff} context). Choices are @code{#UP} (or @code{#1}),
@code{#CENTER} (or @code{#0}) and @code{#DOWN} (or @code{#-1}).

As you can see here, this property can be changed as many times as you
wish. Use @code{\\once \\override} if you don't want the tweak to apply
to the whole score.

"
  doctitle = "Adding a figured bass above or below the notes"
} % begin verbatim

bass = {
  \clef bass
  g4 b, c d
  e d8 c d2
}
continuo = \figuremode {
  <_>4 <6>8
  \once \override Staff.BassFigureAlignmentPositioning #'direction = #CENTER
  <5/>8 <_>4
  \override Staff.BassFigureAlignmentPositioning #'direction = #UP
  <_+>4 <6>
  \set Staff.useBassFigureExtenders = ##t
  \override Staff.BassFigureAlignmentPositioning #'direction = #DOWN
  <4>4. <4>8 <_+>4
}
\score {
  <<
    \new Staff = bassStaff \bass
    \context Staff = bassStaff \continuo
  >>
}


