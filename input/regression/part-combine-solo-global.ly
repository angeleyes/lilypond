
\header { texidoc = "In this example, solo1
	  should not be printed over the 1st note, because of the slur
	  which is present from the one-voice to the two-voice situation."

}

\version "2.1.22"

\score {
    \new Staff
    \partcombine \notes \relative c'' {
	bes2(
	 a4)
	}
    \notes \relative c' {
	r2 cis4
    }
}
