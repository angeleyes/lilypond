\version "2.2.0"
\header {
texidoc = "Jazz chord names can also be printed without notes."
}

\score{
	\context ChordNames \chords{

		\repeat volta 2 {
			f1:maj f:7 bes:7
			c:maj  es
		}
	}
	\paper{
		\context{
			\ChordNamesContext

			\override BarLine #'bar-size = #4

			
			\consists Bar_engraver
			\consists "Volta_engraver"
		}
	raggedright = ##t
	}
}



