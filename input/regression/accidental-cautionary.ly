\version "2.1.28"
\header {

texidoc = "Cautionary accidentals are indicated using either
parentheses (default) or smaller accidentals.

"

}

\score {
    \notes {
    c''4
    cis''?4
    \override Staff.Accidental  #'cautionary-style = #'smaller
    cis''?4
    \override Staff.Accidental  #'cautionary-style = #'parentheses
    cis''?4
    }

\paper { raggedright = ##t }
}

