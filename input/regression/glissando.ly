#(ly:set-option 'old-relative)
\version "2.1.29"

\header{

texidoc=" Between notes, there may be simple glissando lines.
Here, the first two glissandi are not consecutive.

The engraver does no time-keeping, so it involves some trickery to get
<< @{ s8 s8 s4 @} @{ c4 \\gliss d4 @} >> working correctly.

"
}

\score{
        \context Staff=one \notes\relative c''{
		     % gliss non gliss and 
	     c4 \glissando d e \glissando f \glissando \break
	     % consecutive 
	     c \glissando d, \glissando e'
	      << { \stemUp e8 \glissando g8 }
	        \context Voice = VB {\stemDown \repeat unfold 4 d16 } >>
		\override Glissando  #'style = #'zigzag
		c4 \glissando c,, \glissando c' \glissando d
    }
    \paper{
        linewidth = 50.\mm indent = 0
	\context{
	       \StaffContext
	       % makes for handier debugging
	       % \remove Clef_engraver
	}
    }
}

