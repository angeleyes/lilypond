\version "1.7.6"

\header{
texidoc="
Beams over rests.
"
}

\score{
        \context Staff=one \notes\relative c''{
	  r4 [r8 g a]
	  [bes8 r16 f g a]
	  [bes8 r16 \property Voice.stemLeftBeamCount = #1 f g a]
    }

    \paper{
        linewidth =-1
    }
}
%% new-chords-done %%
