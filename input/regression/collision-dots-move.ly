\version "2.1.30"
\header {
    
    texidoc = "If collision resolution finds dotted note head must
	remain on left hand side, move dots to the right."

}

\paper { raggedright = ##t }

\score {
  \notes \relative c {
    \key d \minor
    \clef bass
    << <cis a' cis>4 \\ { g'8. bes16} >>
  }
}
