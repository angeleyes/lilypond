\version "2.11.23"

\header { texidoc = "
Setting the @code{transparent} property will cause an object to be
printed in `invisible ink': the object is not printed, but all its
other behavior is retained.  The object still takes up space, it takes
part in collisions, and slurs, and ties and beams can be attached to it.


The snippet demonstrates how to connect different voices using ties. 
Normally, ties only connect two notes in the same voice.  By
introducing a tie in a different voice, and blanking the first up-stem
in that voice, the tie appears to cross voices. 
" }

\relative c'' {
<< {
  \once \override Stem #'transparent = ##t
  b8~ b8\noBeam
} \\ {
  b[ g8]
} >>
}