

\paper {
  raggedright= ##t
}

hornNotes = \notes \relative c {
		\time 2/4
		R2*3
		r4 f8 a cis4 f e d
}

bassoonNotes = \notes \relative c {
		\clef bass
		r4 d,8 f  gis4 c   b bes 
		a8 e f4   g d  gis f
}


\score {
		\notes {
				\property Score.skipBars = ##t
				\transpose f c'  \hornNotes
		}
}

\score {
		< \context  Staff = hornStaff \hornNotes
		  \context Staff = bsnStaff \bassoonNotes
		>
}
				
