\version "2.1.26"

\header {

texidoc = "In ignatzek inversions, a note is dropped down to act as the
bass note of the chord. Bass note may be also added explicitly.  
Above the staff: computed chord names. Below staff: entered chord name. 
"

}


bladidbla = \chords { 
    f4:maj7/e_":maj7/e" f:maj7/f_":maj7/f" f2:maj7/g_":maj7/g"
    f4:maj7/+e_":maj7/+e" f:maj7/+f_":maj7/+f" f2:maj7/+g_":maj7/+g"
 }

\score {
<< \context ChordNames \bladidbla
  \context Voice \bladidbla >>
}
