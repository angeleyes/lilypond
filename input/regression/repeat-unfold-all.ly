#(ly:set-option 'old-relative)
\version "2.1.7"

\header {
    texidoc = "Repeats may be unfolded through the Scheme function @code{unfold-repeats}."
}

nots = \notes\relative c'   {
    c4 \repeat volta 2 c4 \alternative { d e  }
    \repeat tremolo 4 { c16 d }
}

\score { \notes \context Voice {
\nots
\apply #unfold-repeats \nots
}
\paper {raggedright = ##t} 
     }

