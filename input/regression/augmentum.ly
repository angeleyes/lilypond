\header {
  texidoc = "Augmentum dots are accounted for in horizontal spacing."
}

\version "2.12.0"

\include "gregorian.ly"
\score {
  \new VaticanaVoice {
    \[ \augmentum a \flexa \augmentum g \]
    \augmentum g
  }
}
