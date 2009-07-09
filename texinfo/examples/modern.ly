\version "2.12.0"
\include "example-header.ily"

% parody of modern-style piece, loosely modeled after some Schoenberg
% piano piece. Set res to 104 dpi to get 599 pixels wide

#(ly:set-option 'point-and-click #f)

global = {
  \time 6/8
  \tempo "Ohne Händen, mit den Füßen zu spielen" 4.=66 
  #(set-accidental-style 'dodecaphonic)
  #(set-global-staff-size 18)
}

rhUpper = \relative c'' { 
  \oneVoice
  r4 r16 a32\fff( e' c16\> gis16 g'8. f16 ~ |
  \voiceTwo
  f4.)\mp \oneVoice \times 2/3{fis16( c-.) c->} b8 f'
}

rhLower = \relative c'' {
  \stemUp
  \shiftOn
  s2. 
  b8--[ c8.-- a16--]
}

lhUpper = \relative c {
  c16[ \acciaccatura c16 cis'8. b16 a]
    \clef "treble" \voiceOne bes'8( g8.) aes16 ~ | % 1
  aes4 r8 \clef "bass" %\oneVoice 
%    <a,, gis'>16-.->\ff  <a gis'>16-.-> r8 <a, gis'>-.\pp
  a,16(\f b cis dis f8)
}

lhLower = \relative c' {
  \voiceTwo
  s4. <b f'>8 <bes e>8. <b f'>16 ~ 
  <b f'>4 r8 f8. d
}


\score {
  \new PianoStaff <<
    \new Staff << 	
%      #(set-accidental-style 'dodecaphonic)
      \global
      \context Voice = "upper" { \rhUpper }
      \context Voice = "lower" { \rhLower }
    >>

    \new Staff << 	
      \clef bass 
%      #(set-accidental-style 'modern)
      \global
      \context Voice = "lower-upper" { \lhUpper }
      \context Voice = "lower-lower" { \lhLower }
    >>
  >>
  \layout {}
  \midi {}
}
