% property-init.ly

\version "2.7.39"

stemUp = \override Stem  #'direction = #UP
stemDown = \override Stem  #'direction = #DOWN 
stemNeutral= \revert Stem #'direction

slurUp = \override Slur  #'direction = #UP
slurDown = \override Slur  #'direction = #DOWN
slurNeutral = \revert Slur #'direction 

%% There's also dash, but setting dash period/length should be fixed.
slurDashed = {
  \override Slur #'dash-period = #0.75
  \override Slur #'dash-fraction = #0.4
}
slurDotted = {
  \override Slur  #'dash-period = #0.75
  \override Slur #'dash-fraction = #0.1
}
slurSolid = {
  \revert Slur #'dash-period
  \revert Slur #'dash-fraction
}


phrasingSlurUp = \override PhrasingSlur  #'direction = #UP
phrasingSlurDown = \override PhrasingSlur  #'direction = #DOWN
phrasingSlurNeutral = \revert PhrasingSlur #'direction 

shiftOn = \override NoteColumn  #'horizontal-shift = #1
shiftOnn = \override NoteColumn  #'horizontal-shift = #2
shiftOnnn = \override NoteColumn  #'horizontal-shift = #3
shiftOff = \revert NoteColumn #'horizontal-shift 

tieUp = \override Tie  #'direction = #UP
tieDown = \override Tie  #'direction = #DOWN
tieNeutral = \revert Tie #'direction 

tieDashed = {
  \override Tie #'dash-period = #0.75
  \override Tie #'dash-fraction = #0.4
}
tieDotted = {
  \override Tie #'dash-period = #0.75
  \override Tie #'dash-fraction = #0.1
}
tieSolid = {
  \revert Tie #'dash-period
  \revert Tie #'dash-fraction
}

setEasyHeads = \sequential {
  \override NoteHead  #'stencil = #ly:note-head::brew-ez-stencil
  \override NoteHead #'font-size = #-7
  \override NoteHead #'font-family = #'sans
  \override NoteHead #'font-series = #'bold
}

aikenHeads = \set shapeNoteStyles = ##(do re mi fa #f la ti)

sacredHarpHeads =
  \set shapeNoteStyles = ##(#f #f mi #f fa la #f)

dynamicUp = {
  \override DynamicText  #'direction = #UP
  \override DynamicLineSpanner  #'direction = #UP
}

dynamicDown = {
  \override DynamicText  #'direction = #DOWN
  \override DynamicLineSpanner  #'direction = #DOWN
}

dynamicNeutral = {
  \revert DynamicText #'direction
  \revert DynamicLineSpanner #'direction
}


dotsUp = \override Dots  #'direction = #UP
dotsDown = \override Dots  #'direction = #DOWN
dotsNeutral = \revert Dots #'direction 

tupletUp = \override TupletBracket  #'direction = #UP
tupletDown = \override TupletBracket  #'direction = #DOWN
tupletNeutral = \revert TupletBracket #'direction

cadenzaOn = \set Timing.timing = ##f
cadenzaOff = {
  \set Timing.timing = ##t
  \set Timing.measurePosition = #ZERO-MOMENT
}

% dynamic ly:dir?  text script, articulation script ly:dir?	
oneVoice = #(context-spec-music (make-voice-props-revert) 'Voice)
voiceOne = #(context-spec-music (make-voice-props-set 0) 'Voice)
voiceTwo = #(context-spec-music (make-voice-props-set 1) 'Voice)
voiceThree =#(context-spec-music (make-voice-props-set 2) 'Voice)
voiceFour = #(context-spec-music (make-voice-props-set 3) 'Voice)

	
tiny = 
\set fontSize = #-2

small = 
\set fontSize = #-1

normalsize = {
  \set fontSize = #0
}


%% End the incipit and print a ``normal line start''.
endincipit =  \context Staff {
  \partial 16 s16  % Hack to handle e.g. \bar ".|" \endincipit
  \once \override Staff.Clef  #'full-size-change = ##t
  \once \override Staff.Clef  #'non-default = ##t
  \bar ""
}

autoBeamOff = \set autoBeaming = ##f
autoBeamOn = \set autoBeaming = ##t

fatText = \override TextScript  #'no-spacing-rods = ##f
emptyText = \override TextScript  #'no-spacing-rods = ##t

showStaffSwitch = \set followVoice = ##t
hideStaffSwitch = \set followVoice = ##f



% For drawing vertical chord brackets with \arpeggio
% This is a shorthand for the value of the print-function property 
% of either Staff.Arpeggio or PianoStaff.Arpeggio, depending whether 
% cross-staff brackets are desired. 

arpeggio = #(make-music 'ArpeggioEvent)

arpeggioUp = \sequential {
  \revert Arpeggio  #'stencil
  \override Arpeggio  #'arpeggio-direction = #UP
}
arpeggioDown = \sequential {
  \revert Arpeggio #'stencil
  \override Arpeggio  #'arpeggio-direction = #DOWN
}
arpeggioNeutral = \sequential {
  \revert Arpeggio #'stencil
  \revert Arpeggio  #'arpeggio-direction
}
arpeggioBracket = \sequential {
  \override Arpeggio #'stencil = #ly:arpeggio::brew-chord-bracket
}

glissando = #(make-music 'GlissandoEvent)

fermataMarkup = \markup { \musicglyph #"scripts.ufermata" } 

hideNotes =\sequential {
  % hide notes, accidentals, etc.
  \override Dots  #'transparent = ##t
  \override NoteHead  #'transparent = ##t
  \override NoteHead  #'no-ledgers = ##t
  \override Stem  #'transparent = ##t
  \override Beam  #'transparent = ##t
  \override Accidental  #'transparent = ##t
}


unHideNotes = \sequential {
  \revert Accidental #'transparent
  \revert Beam #'transparent
  \revert Stem #'transparent
  \revert NoteHead #'transparent
  \revert NoteHead #'no-ledgers
  \revert Dots #'transparent 
}

germanChords = {
    \set chordRootNamer = #(chord-name->german-markup #t)
    \set chordNoteNamer = #note-name->german-markup
}
semiGermanChords = {
    \set chordRootNamer = #(chord-name->german-markup #f)
    \set chordNoteNamer = #note-name->german-markup
}

frenchChords = {
    \set chordRootNamer = #(chord-name->italian-markup #t)
    \set chordPrefixSpacer = #0.4
}

italianChords = {
    \set chordRootNamer = #(chord-name->italian-markup #f)
    \set chordPrefixSpacer = #0.4
}

improvisationOn =  {
    \set squashedPosition = #0
    \override NoteHead  #'style = #'slash
}

improvisationOff =  {
    \unset squashedPosition 
    \revert NoteHead #'style
}

textSpannerUp = \override TextSpanner #'direction = #UP
textSpannerDown = \override TextSpanner #'direction = #DOWN
textSpannerNeutral = \revert TextSpanner #'direction

