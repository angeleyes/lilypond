\version "2.1.30"

\header {

    texidoc = "Repeated music can start with grace notes.  Bar checks
    preceding the grace notes do not cause synchronization effects.  "

}

\score{
   \notes\relative c''{\key a \minor \time 2/4
     \repeat "volta" 2 {
     \grace {  a16[ b] }  c4 c4  |
     \grace {  d16[ b] } c4 c4 |
     
     }
   }
   \paper { raggedright = ##t}
 }



