\version "2.1.30"

\header {
    texidoc = " Easy-notation (or Ez-notation) prints names in note heads.
You also get ledger lines, of course."
}

\score {
    \notes {
	\setEasyHeads
	c'2 g'2 | g'1 
	\override NoteHead #'note-names = ##("U" "V" "W" "X" "Y" "Z" "z")
	c'2 e'4 f' | b'1
    }
    \paper {
	raggedright = ##t
    } 
}

