\version "1.7.18"
\header
{
    texidoc = "The autobeamer is not confused by grace notes."
}

\score{
  \notes\context Voice \relative c''{

    \grace a8 g16 f e f \grace a8 g16 f e f \grace a8 g16 f e f 
        \grace a8 g16 f e f |
  }
  \paper { raggedright = ##t }
}

