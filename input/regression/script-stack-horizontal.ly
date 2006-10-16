
\header
{
  texidoc = "horizontal scripts are ordered, so they do not overlap.
The order may be set with script-priority."
  
}
\version "2.9.24"

\paper {
  ragged-right = ##t
}

\relative 
{
  \set stringNumberOrientations = #'(left) 
  \set fingeringOrientations = #'(left) 
  \set strokeFingerOrientations = #'(left) 
  <cis-1\4
   -\rightHandFinger #1 f>\arpeggio
}
