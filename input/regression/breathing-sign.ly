\version "1.7.6"
\header{
texidoc="
Breathing signs, also used for phrasing, do normally not influence
global spacing -- only if space gets tight, notes are shifted to make
room for the breathing sign. Breathing signs break beams running
through their voice. In the following example, the notes in the first
two measures all have the same distance from each other.

Breathing signs are available in different tastes: commas (default),
ticks, vees and `railroad tracks' (caesura).

" }




\score {
  \notes \relative c' {
    \key es \major \time 3/4
    < \context Voice = two { \stemDown es4 bes es }
      \context Voice = one { \stemUp g4 as g }
    > |
    < \context Voice = two { \stemDown es4 \breathe bes es }
      \context Voice = one { \stemUp g4 as g }
    > |
% Change to wedge:
    \property Voice.BreathingSign \override #'text = #"scripts-upbow"
    es8 d es f g8 \breathe f |

    \property Voice.BreathingSign \set #'text = #"scripts-caesura"
    [es8 d] \breathe [es f g f] |
    es2 r4 \bar "||"
  }
}
