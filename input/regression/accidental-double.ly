\version "2.1.36"
\header {
	texidoc = "If two forced accidentals happen at the same time, only one
	sharp sign is printed."
}


\score { \notes \transpose c c'
   \context Staff <<
     \key g \major
     \context Voice=va { \stemUp c' fis! }
     \context Voice=vb { \stemDown c fis! }
   >>

    \paper { raggedright= ##t }
	 
}



