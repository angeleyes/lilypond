
\version "1.9.2"

\header {
texidoc = "Accidentals sticking out to the left
of a note will take a little more space, but only if the spacing is tight." 
}

\score { \notes \relative c'' {
    \time 8/4
    c2 c2 cis2 cis2 |
    c8 c8 cis8 cis8 cis c c c]
}
\paper { raggedright = ##t}

}


