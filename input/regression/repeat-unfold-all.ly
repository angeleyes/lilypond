\version "2.8.0"

\header { texidoc = "Volta repeats may be unfolded through the music
    function @code{\unfoldRepeats}."

}

nots = \relative c'   {
    c4 \repeat volta 2 c4 \alternative { d e  }
}
\layout {ragged-right = ##t} 

\context Voice {
    \nots
    \bar "||"
    \unfoldRepeats \nots
}

