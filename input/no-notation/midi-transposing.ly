#(ly:set-option 'old-relative)
\version "2.2.0"

\header { texidoc = "@cindex Transposing
The transposing property leaves output invariant, but has effect on MIDI. "
}

\score { 
  \context Voice \notes\relative c {
    % btw: this is not how transposing is done in lilypond
	% this is a transposing performer, i.e. for midi-output only
	\set Staff.transposing = #0 c
	\set Staff.transposing = #2 c
	\set Staff.transposing = #4 c
	\set Staff.transposing = #5 c
	\set Staff.transposing = #7 c
	\set Staff.transposing = #9 c
	\set Staff.transposing = #11 c
	\set Staff.transposing = #12 c
	
  }
  \paper { raggedright = ##t } 
  \midi { }
}

