\version "1.7.18"
% remove?  -hwn

\header{ texidoc = "@cindex Two Slurs
You can print a slur in each voice.  DELETE?"
}

% We'd want to combine the stems, but have two slurs too...
% Looks like the a-due engraver



\score{
	\context Staff <
		\context Voice=v \notes\relative c''{
			\stemUp
			a4 c4.(g8-) a4
		}
		\context Voice=u \notes\relative c''{
			\stemDown
			g4 e4.(d8-) c4
		}
	>
	\paper{
		linewidth=60.\mm
	}
}
%% new-chords-done %%
