#(ly:set-option 'old-relative)
\version "1.9.8"

one =  \notes\relative c{
	c'' d e f
}

two =  \notes\relative c{
	\clef "bass"
	c'2 g2
}

\score{
	<<
		\one
		\two
	>>
	\paper{}
	\midi{}
}

% A full example with two staves
%
% Type:
%
%     lilypond example-3
%     xdvi example-3     # or your dvi viewer here

%%
%% For learning LilyPond, please read the tutorial, included in the
%% user-manual.
%% 
