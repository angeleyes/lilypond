%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This is a brief passage from Franz Liszt's %
% transcription for solo piano of Wagner's   %
% Overture to Tannhäuser.                    %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\version "2.12.0"

csu = \change Staff = "high" 
csd = \change Staff = "middle"
sempreff = \markup { \italic "sempre" \dynamic "ff" }

\paper {
  ragged-right = ##f
}

\layout {
  indent = 0.0\cm
}

#(ly:set-option 'point-and-click #f)

global = {
  \key e \major
  #(set-global-staff-size 18)
}

upperVoiceOne = \relative c''' {
  r8 \ottava #1 <dis dis'>16 
  \csd \stemUp <cis, cis'> \csu \stemDown <cis' cis'>[ 
  \csd \stemUp <b, b'>] \csu \stemDown <b' b'>[ 
  \csd \stemUp <gis, gis'>] \csu \stemDown <gis' gis'>[ 
\ottava #0
  \csd \stemUp <dis, dis'>]  \csu \stemDown <dis' dis'>
  \csd \stemUp <cis, cis'>] \csu \stemDown <cis' cis'>[
  \csd \stemUp <b, b'>] \csu \stemDown <b' b'>[
  \csd \stemUp <gis, gis'>] \csu \stemDown %%% END M. 1 %%%
  %%-------------------------------------------%%
  r8 <dis'' dis'>16 
  \csd \stemUp <cis, cis'> \csu \stemDown <cis' cis'>[ 
  \csd \stemUp <b, b'>] \csu \stemDown <b' b'>[ 
  \csd \stemUp <gis, gis'>] \csu \stemDown <gis' gis'>[ 
  \csd \stemUp <fis, fis'>]  \csu \stemDown <fis' fis'>
  \csd \stemUp <dis, dis'>] \csu \stemDown <dis' dis'>[
  \csd \stemUp <cis, cis'>] \csu \stemDown <cis' cis'>[
  \csd \stemUp <b, b'>] \csu \oneVoice %%% END M. 2 %%%
  %%-------------------------------------------%%
  r16 \ottava #1 b'''[_\sempreff b' gis, gis' fis, fis' \ottava #0
    dis,16 dis' cis, cis' b, b' fis, fis'] r | %% END M. 3
  %%-------------------------------------------%%
  r16 \ottava #1 cis'[ cis' gis, gis' fis, fis' \ottava #0
    dis, dis' cis, cis' gis, gis' fis, fis'] r | %% END M. 4
  %%-------------------------------------------%%
  r16 \ottava #1 b[ b' gis, gis' fis, fis' \ottava #0
    dis, dis' b, b' gis, gis' fis, fis'] r | %% END M. 5
  %%-------------------------------------------%%
  r16 \ottava #1 cis'[ cis' gis, gis' fis, fis' e, e' \ottava #0
    cis, cis' gis, gis' e, e'] r | %% END M. 6
}

middleVoiceOne = \relative c' {
  <gis dis' gis>1_^ % 1
  <fis dis' fis>1_^ % 2
  <b dis fis b>2..._^ <cis dis fis cis'>16 | % 3
  <cis dis fis cis'>1_^ | % 4
  <b dis fis b>1_^ | % 5
  <cis e gis cis>1_^ | % 6
}

lowerVoiceOne = \relative c, {
  <b dis gis b>1_^ | % 1
  <b dis fis b>1_^ | % 2
  \voiceOne
  <dis' fis>2... <a dis fis a>16 | % 3
  <a dis fis a>1-^ | % 4
  <a dis fis a>1-^ | % 5
  <gis cis e gis>1-^ | % 6
}

lowerVoiceTwo = \relative c, {
  \voiceTwo
  s1*2
  r4 <b dis fis b>4  <b dis fis b> <b dis fis b> | % 3
  r4 <a dis fis a>4 <a dis fis a> <a dis fis a> | % 4 
  r4 <a dis fis a>4 <a dis fis a> <a dis fis a> | % 5
  r4 <gis cis e gis>4 <gis cis e gis> <gis cis e gis> | % 6
}


\score {
  \new PianoStaff <<
    \new Staff = "high" << 	
      \global
      \context Voice = "upperVoiceOne" { \upperVoiceOne }
    >>
    \new Staff = "middle" << 
      \global
      \context Voice = "middleVoiceOne" { \middleVoiceOne }
    >>
    \new Staff = "low" << 	
      \clef bass 
      \global
      \context Voice = "lowerVoiceOne" { \lowerVoiceOne }
      \context Voice = "lowerVoiceTwo" { \lowerVoiceTwo }
    >>
  >>
  \layout {
    \context {
      \Score
      \override TimeSignature #'stencil = ##f
      \remove "Bar_number_engraver"
    }
  }
  %{\midi {
    \context {
      \Score
      tempoWholesPerMinute = #(ly:make-moment 120 4)
    }
  }%}
}
