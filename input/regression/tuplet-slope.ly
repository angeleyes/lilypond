\header {

    texidoc = "Tuplet brackets stay clear of the staff. The
slope is determined by the graphical characteristic of the notes, but
if the musical pattern does not follow graphical slope, then the
bracket is horizontal

The bracket direction is determined by the dominating stem direction.
 
"

    }
 

\version "2.1.12"

\score {
  \notes \relative c' {
    \times 4/5 { a'4 as g fis f }
    \times 4/5 { fis4 e es d des }
    \times 4/5 { fis,4 e es d des }
    \times 4/5 { bes'''4 bes,, b c cis }
    \times 4/5 { a''4 b b c cis }
  }

  \paper {
    linewidth = -1
  }
}

% EOF
