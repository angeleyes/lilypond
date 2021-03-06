%% Do not edit this file; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.13.10"

\header {
  lsrtags = "pitches"

%% Translation of GIT committish: b2d4318d6c53df8469dfa4da09b27c15a374d0ca
doctitlees = "Evitar que se añadan becuadros adicionales automáticamente"

texidoces = "
Según las reglas estándar de composición
tipográfica, se imprime un becuadro antes de un sostenido o un
bemol cuando se tiene que cancelar una alteración anterior en la
misma nota.  Para modificar este comportamiento, establezca el
valor de la propiedad @code{extraNatural} a @code{##f} (falso)
dentro del contexto de @code{Staff}.

"

%% Translation of GIT committish: d96023d8792c8af202c7cb8508010c0d3648899d
  doctitlede = "Verhindern, dass zusätzliche Auflösungszeichen automatisch
hinzugefügt werden"

texidocde = "Den tranditionellen Notensatzregeln zufolge wird ein
Auflösungszeichen immer dann vor einem Kreuz oder B gesetzt, wenn
ein vorheriges Versetzungszeichen der gleichen Note aufgehoben werden
soll.  Um dieses Verhalten zu ändern, muss die Eigenschaft
@code{extraNatural} im @code{Staff}-Kontext auf \"false\" gesetzt werden.
"
%% Translation of GIT committish: 59968a089729d7400f8ece38d5bc98dbb3656a2b
  texidocfr = "
En accord avec les règles standards de l'écriture musicale, on grave
un bécarre avant un dièse ou un bémol si on a besoin d'annuler une
altération précédente.  Pour modifier ce comportement, assignez la propriété
@code{extraNatural} du contexte @code{Staff} à la valeur @code{##f} (faux).
"

  doctitlefr = "Suppression des bécarres superflus"

  texidoc = "
In accordance with standard typesetting rules, a natural sign is
printed before a sharp or flat if a previous accidental on the same
note needs to be canceled.  To change this behavior, set the
@code{extraNatural} property to @code{f} in the @code{Staff} context.



"
  doctitle = "Preventing extra naturals from being automatically added"
} % begin verbatim

\relative c'' {
  aeses4 aes ais a
  \set Staff.extraNatural = ##f
  aeses4 aes ais a
}

