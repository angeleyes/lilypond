\version "2.1.36"
\header  {
    texidoc = "@cindex Chord Names German
The english naming of chords (default) can be changed to german 
(@code{\germanChords} replaces B and Bes to H and B) or semi-german 
(@code{\semiGermanChords} replaces B and Bes to H and Bb).

" }

scm = \chords {
    c1/c cis/cis
    % yeah, we get the idea. -hwn
    
    % cisis/cisis ces/ces ceses/ceses
    b/b bis/bis bes/bes
    % beses/beses
} 
\score {
\notes <<
    \context ChordNames { \scm }
    \new ChordNames {
	\set instrument = #"german"
	\germanChords \scm }
    \new ChordNames {
	\set instrument = #"semi-german"
	\semiGermanChords \scm }
    \context Voice {  \scm } >>
\paper {
    raggedright = ##t 
    \context {\ChordNamesContext \consists Instrument_name_engraver }}

}
