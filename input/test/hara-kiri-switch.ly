
\header {
texidoc = "some instruments (notably: cello and double bass) are alternatively written on one and two staves. This is the way to do it:"
}
\version "1.7.18"


voiceOne =  \notes \relative c'' {
	a1 a a
	a2 a2
	a2 a8 a a a
}

staffOne =  \context Staff = one <
	\notes \context Voice=one < 
		\voiceOne
	>
>

voiceTwo =  \notes \relative c' {
	% we must have a Thread context before we can switch staves
	% in this case, the notes before the switching  will do that 
	% implicitely
	\context Thread

	R1*3
	c2 c2
	\translator Staff=one
	\translator Voice=one
	c2
	c8 c c c
}

staffTwo =  \context Staff = two <
	% notes must go to Staff context: we want to switch Staff translators
	%\notes \context Voice=one < 
		\voiceTwo
	%>
>

grandstaff =  \context GrandStaff = one <
	\staffOne
	\staffTwo
>

\score{
	\grandstaff
	\paper{
		linewidth=80.0\mm

		\translator { \VoiceContext autoBeaming = ##f }
		\translator { \HaraKiriStaffContext }
		\translator { \OrchestralScoreContext skipBars = ##t }
	}
}


%% new-chords-done %%
