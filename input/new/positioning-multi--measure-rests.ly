\version "2.11.42"
\layout { ragged-right= ##t }
\header {
  doctitle = "Positioning multi-measure rests"
  lsrtags = "rhythms,tweaks-and-overrides"
  texidoc = "
Unlike ordinary rests, there is no predefined command
to change the vertical position on the staff of a multi-measure
rest symbol of either form by attaching it to a note.  However,
in polyphonic music multi-measure rests in odd-numbered and
even-numbered voices are vertically separated.  The positioning
of multi-measure rests can be controlled as follows:
 "}

\relative c'' {
% Multi-measure rests by default are set under the 2nd line
R1
% They can be moved with an override
\override MultiMeasureRest #'staff-position = #-2
R1
% A value of 0 is the default position;
% the following trick moves the rest to the center line
\override MultiMeasureRest #'staff-position = #-0.01
R1
% Multimeasure rests in odd-numbered voices are under the top line
<< {R1} \\ {a1} >>
% Multi-measure rests in even-numbered voices are under the bottom line
<< {c1} \\ {R1} >>
% They remain separated even in empty bars
<< {R1} \\ {R1} >>
% This brings them together even though there are two voices
\compressFullBarRests
<<
  \revert MultiMeasureRest #'staff-position
  {R1*3}
\\
  \revert MultiMeasureRest #'staff-position
  {R1*3}
>>
}
