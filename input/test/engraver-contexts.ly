\version "2.3.17"
\header {
    
texidoc ="In polyphonic notation, many voices can share a staff: In
this situation, the accidentals and staff are shared, but the stems,
slurs, beams, etc. are private to each voice. Hence, engravers should
be grouped. The engravers for note head, stems, slurs, etc. go into a
group called ``Voice context'', while the engravers for key,
accidental, bar, etc. go into a group called ``Staff context''. In the
case of polyphony, a single Staff context contains more than one Voice
context. Similarly, more Staff contexts can be put into a single Score
context. "

}

\include "engraver-example.lyinc"

\score {
\context Staff << \topVoice \\ \botVoice >>
}


\score {
<< \new Staff << \topVoice \\ \botVoice >>
\new Staff << \pah \\ \hoom >>
  >>
}



