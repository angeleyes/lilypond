
\version "2.3.22"
\header{
    texidoc="@cindex Beam Position Control

Beam positions may be controlled manually, by overriding the @code{positions} setting of the @code{Beam} grob.

" }
\score { 
    \context Voice \relative c {
	%% from upper staffline (position 4) to centre (position 0)
	\override Beam  #'positions = #'(2 . 0)
	 c'8[ c] 
	
	%% from center to one above centre (position 2)
	\override Beam  #'positions = #'(0 . 1)
	 c[ c]
  }
\layout{raggedright = ##t}
}

