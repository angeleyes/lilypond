\version "1.7.6"
\score{
	\context PianoStaff <
	\context Staff=one \notes\relative c'{
		s1
	}
	\context Staff=two \notes\relative c'{
		\clef bass
% no knee
		\stemUp [c8 \translator Staff=one \stemDown g'16 f]
		s8
		s2
	}
	>
	\paper{
 		linewidth=-1.
	}
}


%% new-chords-done %%
