\version "2.9.13"

\header{
    texidoc="If there are no good places to have a page turn,
the optimal-breaker will just have to recover gracefully. This
should appear on 3 pages.
"
}

\paper {
  #(define page-breaking ly:page-turn-breaking)
  paper-height = #70
}

\relative c' {
  a b c d a b c d \break
  c d e f c d e f \break
  d e f g d e f g
}


