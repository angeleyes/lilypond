\version "1.7.18"

\score{
	\context Staff = one <
		\context Voice=one { \skip 1 }
		\context Voice=two { \skip 1 }

		\context Voice=one \partcombine Voice
			\context Thread=one \notes\relative c''
				{
					e \property Voice.soloADue = ##f e
				}
			\context Thread=two \notes\relative c''
				{
					e \property Voice.soloADue = ##f e
				}
		>
	\paper{
		linewidth = 100.\mm
		raggedright=##t
	}
}
%% new-chords-done %%
