%
% We'd want to combine the stems, but have two slurs too...
% Looks like the a-due engraver
%
\score{
	\context Staff <
		\context Voice=v \notes\relative c''{
			\stemup
			a4 c4.()g8 a4
		}
		\context Voice=u \notes\relative c''{
			\stemdown
			g4 e4.()d8 c4
		}
	>
	\paper{
		linewidth=60.\mm;
	}
}
