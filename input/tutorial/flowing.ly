\version "1.7.6"

\header {
        title = "The river is flowing"
        composer = "Traditonal (?)"
}

\include "paper16.ly"

melody = \notes \relative c' {
        \partial 8
        g8 |
        c4 c8 d es-[ ( d-]-) c4 | f4 f8 g es-(-[ d-)-] c g |
        c4 c8 d es-[ ( d-]-) c4 | d4 es8 d c4.
        \bar "|."
}

text = \lyrics {
        The ri -- ver is flo- __ wing, flo -- wing and gro -- wing, the
        ri -- ver is flo -- wing down to the sea.
}

accompaniment =\chords {
        r8
        c2:3- f:3-.7 d:m es4 c8:m r8
        c2:m f:m7 g:7^3.5 c:m }

\score {
        \simultaneous {
%	  \accompaniment
          \context ChordNames \accompaniment

          \addlyrics
             \context Staff = mel
             {	\property Staff.autoBeaming = ##f
		\property Staff.automaticMelismata = ##t
          	\melody }
             \context Lyrics \text
        }
        \midi  { }
        \paper { linewidth = 10.0\cm }
}
