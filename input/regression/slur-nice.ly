#(ly:set-option 'old-relative)
\version "1.9.0"
\header{
texidoc="
Slurs should look nice and symmetric.  The curvature may increase
only to avoid noteheads, and as little as possible.  Slurs never
run through noteheads or stems.
"
}

\score{
	\notes\relative c''{
		\time 3/4
		\slurUp
		\stemBoth a ( \stemDown a \stemBoth  a-) a( c  a-) a( e'  a,-) a( g'  a,-)
		\stemUp a( e'  a,-)
		\break
		\slurDown
		\stemBoth c ( \stemUp c \stemBoth  c-) c ( a  c-) c( d,  c'-) c( f,  c'-)
		\stemDown c( f,  c'-)
	}
	\paper{
		linewidth = 120.\mm
	}
}

