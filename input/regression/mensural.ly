\version "2.3.4"
\header {
    
texidoc = "There is limited support for mensural notation: note head
shapes are available. Mensural stems are centered on the note heads,
both for up and down stems."

}


\score { { \context Voice { 
    \override NoteHead  #'style = #'mensural
\transpose c d'' {  c4 c2 c8  c16 c16  c1 c\breve c\longa }
\transpose c c' { c4 c2 c8  c16 c16  c1 c\breve c\longa }
}}}

