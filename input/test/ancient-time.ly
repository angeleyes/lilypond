\version "2.1.36"
% TODO: move stuff from ancient-font into here?  See comment
% for ancient-font.ly
\header {
texidoc="@cindex Ancient Time Signatures
Time signatures may also be engraved in an old style.
"
}

\score {
  \notes { 
    \override Staff.TimeSignature  #'style = #'neo_mensural
    s1 
  }
	\paper {raggedright = ##t}
}

