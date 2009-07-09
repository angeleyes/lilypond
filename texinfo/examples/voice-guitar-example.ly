\version "2.12.0"
\include "example-header.ily"

%%********************************************************%
% An override beam position definition using two numbers %
%********************************************************%
slant = 
#(define-music-function (parser location positionA positionB) (number? number?)
  #{
    \once \override Beam #'positions = #(cons $positionA $positionB )
  #})

% example: \slant #5 #6


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

caracolaGlobal = {
  \time 3/4
}

caracolaVoiceNotes = \relative c'' {
  \clef treble
  \caracolaGlobal
  g,4^\mf c d | % 12
  e8( fis e4) e | % 13
  d8( e d4) d |	% 14
  c4 g2 | % 15
}

caracolaText =  \lyricmode {
  den -- tro le can -- ta~un mar de 
}

caracolaMelody =  \relative c {
  \voiceOne
  \caracolaGlobal
  \override Fingering #'staff-padding = #'()
%% This line allows for multiple triplets inside a single \times command
\set tupletSpannerDuration = #(ly:make-moment 1 4)
\slant #2 #3.4
  \times 2/3{<c-\tweak #'extra-offset #'( -0.3 . -1.7 )-1>8\(\<[ 
   <g'-\tweak #'extra-offset #'( -0.2 . -0.2 )-3> 
   <c-\tweak #'extra-offset #'( -0.1 . -0.0 )-4>] 
   g[ c <d-\tweak #'extra-offset #'( -0.2 . -0.0 )-2>] 
\stemDown
   c[ d <e-\tweak #'extra-offset #'( -0.2 . -0.2 )-0>\)]} | % 12 
\stemNeutral
\override TupletNumber #'stencil = ##f
  \times 2/3{g,8\([ c d] c[ d e] g,[ c d\)\!]}	|	% 13
  \times 2/3{bes,8\([ f'\> bes] f[ bes c] bes[ c e\)]} |	% 14
  \times 2/3{a,,8\([ e' aes] e[ aes c] aes[ c e\)]\!} |	% 15
}

caracolaBass =  \relative c {
  \voiceTwo
  \caracolaGlobal
}

\paper {
  #(set-paper-size "letter")
  ragged-right = ##t
  indent = 0\cm
}

\score {
  \new StaffGroup = "voiceAndGuitar" <<
    \context Staff = "voice" <<
      \set Staff.midiInstrument = "flute"
      %\set Staff.instrumentName = "Voice"
      \context Voice = "voiceNotes" { \caracolaVoiceNotes }
      \new Lyrics { \lyricsto "voiceNotes" { \caracolaText } }
      %\addlyrics { \caracolaText }
    >>  
    \context Staff = "guitar" \with { 
      \consists "Span_arpeggio_engraver" 
    }
    << 
      \set Staff.midiInstrument = "acoustic guitar (nylon)"
      %\set Staff.instrumentName = "Guitar"
      \set Staff.connectArpeggios = ##t
      \clef "treble_8"
      \context Voice = "melody" { \caracolaMelody }
      \context Voice = "bass"   { \caracolaBass   }
    >>
  >> % end of voiceAndGuitar staffgroup

  \layout { }
  
  \midi {
    \context {
      \Score
    %  tempoWholesPerMinute = #(ly:make-moment 160 4)
    }
  }
}


