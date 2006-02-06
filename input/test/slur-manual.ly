\version "2.7.32"
\header {


    texidoc = "In extreme cases, you can resort to setting the 
    @code{control-points} of a slur manually, althout it involves 
    a lot of trial and error. Be sure to force line breaks at both sides, since
    different horizontal spacing will require rearrangement of the
    slur."
 
    }

\score {
     \new PianoStaff
    <<
	\context Staff = "up" {
	    \clef bass
	    s1 * 4
	} 
	\context Staff = "down" \relative c <<
	    s1*4
	    {
	    \clef bass
	    r4 r8
	    \once\override Slur  #'extra-offset = #'(0 . 6)
	    \once\override Slur  #'control-points =
	    #'((0 . -4) (2 . 0) (60 . 0) (63 . 4))
	    c8( as' f c' as

	    % line breaks will mess up this example  
	    \noBreak
	    f c as' f
	    \change Staff = up
	    \clef treble
	    c' as f' c
    	    \noBreak
	    as' f c' as
	    f' c as' f c'4)
	}>>
    >>
    \layout {  }
}

