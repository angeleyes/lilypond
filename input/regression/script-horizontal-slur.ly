\version "2.10.20"

\header {
  texidoc = "Horizontal scripts don't have @code{avoid-slur} set."
}

\layout {
  ragged-right = ##t
}

\relative c'' { 
  \set fingeringOrientations = #'(right)
  < a-1 d-2>2
  < a-1 d-2>2
  < a-1 d-2>2(
  < a-1 d-2>2)
}
