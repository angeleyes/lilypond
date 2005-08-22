\version "2.6.0"
\header  {
  texidoc = "Grace notes in different voices/staves are synchronized."
}

\layout { raggedright = ##t}


\relative c'' <<
  \context Staff  {
    c2
				   \grace  c8
				   c4 c4 }
  \new Staff {
    c2 \clef bass
    \grace {  dis8[ ( d8] \key es\major  }

    c4) c4 }
  \new Staff { c2 c4 c4 \bar "|." }
>>



