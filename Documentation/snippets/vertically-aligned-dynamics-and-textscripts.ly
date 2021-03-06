%% Do not edit this file; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.13.10"

\header {
  lsrtags = "tweaks-and-overrides, spacing"

%% Translation of GIT committish: b2d4318d6c53df8469dfa4da09b27c15a374d0ca
  texidoces = "
Mediante el establecimiento de la propiedad @code{'Y-extent} a un valor
adecuado, todos los objetos @code{DynamicLineSpanner} (reguladores e
indicaciones dinámicas textuales) se pueden alinear con un punto de
referencia común, independientemente de sus dimensiones.  De esta
manera, todos los elementos estarán alineados verticalmente,
produciendo así un resultado más satisfactorio.

Se usa una idea similar para alinear las inscripciones de texto a lo
largo de su línea de base.

"
  doctitlees = "Indicaciones dinámicas y textuales alineadas verticalmente"

%% Translation of GIT committish: d96023d8792c8af202c7cb8508010c0d3648899d
texidocde = "
Indem man die @code{'Y-extent}-Eigenschaft auf einen passenden Wert setzt,
können alle @code{DynamicLineSpanner}-Objekte (Crescendo-Klammern und
Dynamik-Texte)  (hairpins and dynamic texts) unabhängig von ihrer
wirklichen Ausdehnung an einem gemeinsamen Referenzpunkt ausgerichtet werden.
Auf diese Weise ist jedes Element vertikal ausgerichtet und der Notensatz
sieht ansprechender aus.

Die gleiche Idee wird benutzt, um Textbeschriftungen an ihrer
Grundlinie auszurichten.

"
  doctitlede = "Vertikale Ausrichtung von Dynamik und Textbeschriftung beeinflussen"
%% Translation of GIT committish: ae814f45737bd1bdaf65b413a4c37f70b84313b7
  texidocfr = "
Vous pourrez, en jouant sur la propriété @code{'Y-extent}, aligner les
différents objets @code{DynamicLineSpanner} (souflets ou textuels)
quelle que soit leur étendue, par rapport à un même point de référence.
Tous les éléments seront alors centrés sur une même ligne, ce qui sera
visuellement plus agréable.

C'est le même principe qui sert à aligner les indications textuelles sur
une ligne de référence.

"
  doctitlefr = "Alignement vertical des nuances indications textuelles"


  texidoc = "
By setting the @code{'Y-extent} property to a suitable value, all
@code{DynamicLineSpanner} objects (hairpins and dynamic texts) can be
aligned to a common reference point, regardless of their actual extent.
This way, every element will be vertically aligned, thus producing a
more pleasing output.

The same idea is used to align the text scripts along their baseline.

"
  doctitle = "Vertically aligned dynamics and textscripts"
} % begin verbatim

music = \relative c'' {
  c2\p^\markup { gorgeous } c\f^\markup { fantastic }
  c4\p c\f\> c c\!\p
}

{
  \music \break
  \override DynamicLineSpanner #'staff-padding = #2.0
  \override DynamicLineSpanner #'Y-extent = #'(-1.5 . 1.5)
  \override TextScript #'Y-extent = #'(-1.5 . 1.5)
  \music
}

