% possible rename to ancient- or gregorian- ?
\header {
    texidoc = "@cindex Divisiones

Divisiones are ancient variants of breathing signs.
Choices are @code{divisioMinima}, @code{divisioMaior},
@code{divisioMaxima} and @code{finalis}, @code{virgula} and
@code{caesura}.

" }

\version "2.7.13"

\include "gregorian-init.ly"

\score {
  <<
    \context VaticanaVoice {
      \override Staff.StaffSymbol #'color = #red
      \override TextScript  #'padding = #3
      g a g
      s^\markup { "divisio minima" }
      \divisioMinima
      g a g
      s^\markup { "divisio maior" }
      \divisioMaior
      g a g
      s^\markup { "divisio maxima" }
      \divisioMaxima
      \break
      g a g
      s^\markup { "finalis" }
      \finalis
      g a g
      s^\markup { "virgula" }
      \virgula
      g a g
      s^\markup { "caesura" }
      \caesura
      g a g
    }
  >>
}
