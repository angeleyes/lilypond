
\version "2.1.28"
\header{
texidoc="
Key signatures may appear on key changes, even without a barline.  
In the case of a line break, the restoration accidentals are printed at 
end of a line. If @code{createKeyOnClefChange} is set, key signatures
are created also on a clef change.
"
}



\score {
  \notes \relative c''
  {
	\set Staff.createKeyOnClefChange = ##t  
    \key bes \major c2
%    \key c \major %  \minor
    \key es \major %  \minor
    c2
    \break
    \key bes \major % \major
    c2 \clef alto c2   \key d \major \clef treble c2
  	\set Staff.keySignature = #'((2 . -1)  (6 . 3) (4 . -2))
	e2
  }
}

