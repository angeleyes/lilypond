\version "1.7.5"
\header{
texidoc="
Chord names are generated from a list pitches, and are customisable
from guile.  For some unlogical names, guile customisation is used
by default.
"
}

chord =  \notes\transpose c c'\chords{
   c1
   c:m
   c:m5-
   c:m5-.7-
   c:7+
   c:m5-.7
   c:5-.7+
   c:m7
   c:7
   d
   d/a
   d/+gis
}

\score{
    <
	    \context ChordNames \chord
	    \context Staff \chord
    >
    \paper{

        	\translator { 
			\ChordNamesContext
			ChordName \override #'word-space = #1 
		}
    }
}

%% new-chords-done %%