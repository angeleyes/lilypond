\version "2.1.7"
\header {

    texidoc =
"

In improvisation, noteheads do not have a pitch, and have different
shapes. In this example, this is achieved by adding
@code{Pitch_squash_engraver} and setting @code{squashedPosition} when the
improvisation is active.

"

}


improOn = \notes {
    \property Voice.squashedPosition = #0
    \property Voice.NoteHead \override #'style = #'slash
}

improOff = \notes {
    \property Voice.squashedPosition \unset
    \property Voice.NoteHead \revert #'style
}

global = \notes { s1*3 \bar "|." }

\score {
    <<
	\context ChordNames \chords {
	    e8*7:m7 a2.:m7 bes4:m7 b1:m7 e8:m
	}
	\notes <<
	    \context Voice = melo \transpose c c' {
		e8 e g a a16(bes)(a8) g \improOn
		e8
		~e2~e8 f4 fis8
		~fis2 \improOff a16(bes) a8 g e
	    }
	>>
    >>
    \paper { 
	\translator {
	    \VoiceContext
	    \consists Pitch_squash_engraver
	}
	raggedright = ##t
    }
}

