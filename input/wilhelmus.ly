\version "2.5.2"

\header {
  texidoc = "Wilhelmus van Nassouwe"
  title = "Wilhelmus van Nassouwe"
  composer = "Valerius"
  subtitle = "Neder-landtsche gedenck-clanck"
  opus = "1626"
  copyright = "public domain"
  enteredby = "janneke@gnu.org"
}

%% hymn tricks
#(define (prepend-grob-property grob-name
	  grob-prop entry)
  (lambda (context)
   (let*
    ((grob-def (ly:context-property context grob-name)))
    (if (pair? grob-def)
     (ly:context-pushpop-property context grob-name grob-prop
      (cons
       entry
       (assoc-get grob-prop (car grob-def))))))))

#(define (set-extra-space grob-name entry value)
  (prepend-grob-property grob-name 'space-alist
   (cons entry (cons 'extra-space value))))

noclefs =  {
  s1
  \override Staff.Clef #'break-visibility = #(lambda (dir) (cons #t #t))
}

setMargins = {
  %% first line left margin
  %% justified:
  %% \context Staff \applycontext #(set-extra-space 'TimeSignature 'first-note 4.5)
  %% raggedright:
  \context Staff \applycontext #(set-extra-space 'TimeSignature 'first-note 9.5)
  
  %% next lines left margin
  \context Staff \applycontext #(set-extra-space 'KeySignature 'staff-bar 15)
  
  %% next lines small key-signature margin
  \context Staff \applycontext #(set-extra-space 'LeftEdge 'key-signature 1.0)
}

pipeSymbol = {
  %% Set height of bar line to 2 staff-spaces
  \once \override Staff.BarLine #'bar-size-procedure = #(lambda (x) 2)
  %% Move barline one staff-space up
  \once \override Staff.BarLine #'extra-offset = #'(0 . 1)
  \bar "|"
}

myBreak = { \bar "" \break }

\layout {
  indent = 0.0\mm
  linewidth = 120.0\mm
  textheight = 270.0\mm
}

voice =  \relative c' {
  \clef violin
  \key g \major
  d4 | g g a a b | a8 \myBreak
  b8 | c4 b a a | g2. \myBreak

  d4 | g g a a | b a8 \myBreak
  b8 | c4 b a a| g2.  \myBreak

  %% Hack for better left tekst margin
  %% b8[ c] | d2 e4 d2 c4 | b a8 \myBreak
  \set melismaBusyProperties = #'()
  b8[ c] |
  \unset melismaBusyProperties
  d2 e4 d2 c4 b | a8 \myBreak
  b8 | c4 b a g | a2. \myBreak

  d,4 | g4.\melisma a8\melismaEnd b2 a2 g4 | fis e8 \myBreak
  d8 | e4 g g fis | 
  
  \override NoteHead #'style = #'neomensural

  g\breve

  %% justified lines:
  %%\override Staff.BarLine #'extra-offset = #'(12 . 0)
  %% raggedright:
  \override Staff.BarLine #'extra-offset = #'(23 . 0)
  \bar "|."
}

stich =  \relative c'' {
  \override Staff.NoteCollision #'merge-differently-dotted = ##t
  \set fontSize = #-3
  %% broken?
  \override Stem #'beamed-lengths = #(map (lambda (x) (* 0.2 x)) '(3.26))

  \voiceTwo
  \partial 4
  s4 s s a8[ b] c[ a] s4.
  s8 s4 s a8 g s4 s2.

  s4 s s a8[ b] c[ a] s4.
  s8 s4 s a8 g s4 s2.
  
  s8 s s2 s4 s2 s4 s s8
  s8 s4 s s s s2.

  s4 g8[ fis g a] s2 s2 s4 s s8
  s8 e8[ fis]
}

modernText = \lyricmode {
  Wil -- hel -- mus van Nas -- sou -- we
  ben ik van duit -- sen bloed,

  den va -- der -- land ge -- trou -- we
  blijf ik tot in den dood.

  Een prin -- se van O -- ran -- je
  ben ik vrij on -- ver -- veerd,

  den ko -- ning van His -- pan -- je
  heb ik al -- tijd ge -- eerd.
}

text = \lyricmode {
  Wil -- hel -- mus van Nas -- sou -- we
  Ben ick van duyt -- schen bloet,
  Den Va -- der -- landt ghe -- trou -- we,
  blyf ick tot in den doot!
  %% Hack for better left text margin:
  %% Een Prin -- ce van O -- ran -- gien
  Een " " Prin -- ce van O -- ran -- gien
  Ben ick, vry, on -- ver -- veert;
  Den Co -- ninck van His -- pan -- gien
  Heb ick al -- tijt ghe -- eert.
}

oneHalfNoteTime = \markup {
  \override #'(baseline-skip . 0)
  \column { { \number "1" } { \smaller \smaller \note #"2" #-0.5 } }
}

\layout {
  raggedright = ##t

  \context {
    \Score
    %% defaults
    %% (shortest-duration-space . 2.0)
    %% (spacing-increment . 1.2)
    %% (base-shortest-duration . ,(ly:make-moment 1 8))
    %% wider spacing
    \override SpacingSpanner #'shortest-duration-space = #3.0
    \override SpacingSpanner #'spacing-increment = #1.2
    \override SpacingSpanner #'base-shortest-duration = #(ly:make-moment 1 8)
  }
}

\score {
  \context Score <<
    \context Staff <<
      \override Staff.StaffSymbol #'width = #'80
      \set Staff.autoBeaming = ##f
      \set Score.timing = ##f
      \setMargins
     
      %% Less vertical space needed with lyrics
      \set Staff.minimumVerticalExtent = #'(2 . 2)
      
      %% Custom time signature
      \override Staff.TimeSignature #'print-function = #Text_interface::print
      \override Staff.TimeSignature #'text = #oneHalfNoteTime
      \noclefs
    
      \context Voice = "voice" \voice
      \context Voice = "stich" \stich
    >>
    \lyricsto "voice" \new Lyrics {
      \set minimumVerticalExtent = #'(2 . 2)
      \text
    }
  >>
  \layout {}
  \midi {}
}

%%% Local variables:
%%% LilyPond-indent-level:2
%%% End:
