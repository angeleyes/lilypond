
\version "2.2.0"
\header{
	texidoc="@cindex Beam Count

You can alter the number of stems in a beam.  In this example,
two sets of four 32nds are joined, as if they were 8th notes.

" }

fragment = \notes {
  #(override-auto-beam-setting '(end * * * *)  1 4)
  f32 g a b b a g f

  f32 g a 
  \set stemRightBeamCount = #1  b
  \set stemLeftBeamCount = #1 b
  a g f
}


\score {
  \notes\relative c \fragment
  \paper { raggedright = ##t}  
}

