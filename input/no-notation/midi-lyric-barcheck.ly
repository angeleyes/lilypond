\version "2.1.36"
\header {

    texidoc = "Lyrics in MIDI are aligned to ties and beams:
this examples causes no bar checks in MIDI.
"


    }
\score {

    <<\notes\relative c'' \context Voice = A {
	\autoBeamOff
	c8[ c] c2.
	c1~c4 c2.
	c4 ( d e) d
	c1

    }
    \lyricsto "A" \lyrics\new Lyrics { bla bla | bla bla | bla bla | bla }
    >>
    \paper {}
      \midi {}
    }
