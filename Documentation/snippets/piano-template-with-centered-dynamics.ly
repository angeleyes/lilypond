% Do not edit this file; it is automatically
% generated from Documentation/snippets/new
% This file is in the public domain.
%% Note: this file works from version 2.13.4
\version "2.13.4"

\header {
%% Translation of GIT committish: b2d4318d6c53df8469dfa4da09b27c15a374d0ca
  texidoces = "
Muchas partituras de piano tienen las indicaciones dinámicas
centradas entre los dos pentagramas.  Esto requiere un poco de
trucaje, pero puesto que la plantilla está aquí mismo, no tenemos
que hacer este trucaje por nuestra cuenta.

"
  doctitlees = "Plantilla de piano con matices centrados"


%% Translation of GIT committish: 06d99c3c9ad1c3472277b4eafd7761c4aadb84ae
  texidocja = "
多くのピアノ譜は、強弱記号を 2 つの譜の間に置きます。これを実現するにはちょっ@c
とした調整が必要ですが、そのテンプレートがここにあるので、あなた自身が調整を行@c
う必要はありません。
"

%% Translation of GIT committish: d96023d8792c8af202c7cb8508010c0d3648899d
  texidocde = "
In der meisten Klaviernotation werden die Dynamikzeichen zwischen
den beiden Systemen zentriert. Für LilyPond muss man die Einstellungen
etwas anpassen, aber Sie können ja das angepasste Beispiel von hier
kopieren.
"


%% Translation of GIT committish: 9ba35398048fdf1ca8c83679c7c144b1fd48e75b
  texidocfr = "
Nombre d'ouvrages pour piano font apparaître les nuances entre les deux
portées.  Bien que cela nécessite quelques subtilités, voici de quoi
obtenir un tel résultat.

"
  doctitlefr = "Piano et nuances entre les portées"

  lsrtags = "expressive-marks, keyboards, template"
  texidoc = "
Many piano scores have the dynamics centered between the two staves.
This requires a bit of tweaking to implement, but since the template is
right here, you don't have to do the tweaking yourself.

"
  doctitle = "Piano template with centered dynamics"
} % begin verbatim


global = {
  \key c \major
  \time 4/4
}

upper = \relative c'' {
  \clef treble
  a4 b c d
}

lower = \relative c {
  \clef bass
  a2 c
}

dynamics = {
  s2\fff\> s4 s\!\pp
}

pedal = {
  s2\sustainOn s\sustainOff
}

\score {
  \new PianoStaff = "PianoStaff_pf" <<
    \new Staff = "Staff_pfUpper" << \global \upper >>
    \new Dynamics = "Dynamics_pf" \dynamics
    \new Staff = "Staff_pfLower" << \global \lower >>
    \new Dynamics = "pedal" \pedal
  >>
  \layout { }
}

\score {
  \new PianoStaff = "PianoStaff_pf" <<
    \new Staff = "Staff_pfUpper" << \global \upper \dynamics \pedal >>
    \new Staff = "Staff_pfLower" << \global \lower \dynamics \pedal >>
  >>
  \midi { }
}