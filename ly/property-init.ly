% property-init.ly

\version "2.1.26"

stemUp = \override Stem  #'direction = #1
stemDown = \override Stem  #'direction = #-1 
stemBoth= \revert Stem #'direction

slurUp = \override Slur  #'direction = #1
slurDown = \override Slur  #'direction = #-1
slurBoth = \revert Slur #'direction 

% There's also dash, but setting dash period/length should be fixed.
slurDotted = \override Slur  #'dashed = #1
slurSolid = \revert Slur #'dashed


phrasingSlurUp = \override PhrasingSlur  #'direction = #1
phrasingSlurDown = \override PhrasingSlur  #'direction = #-1
phrasingSlurBoth = \revert PhrasingSlur #'direction 

shiftOn = \override NoteColumn  #'horizontal-shift = #1
shiftOnn = \override NoteColumn  #'horizontal-shift = #2
shiftOnnn = \override NoteColumn  #'horizontal-shift = #3
shiftOff = \revert NoteColumn #'horizontal-shift 

tieUp = \override Tie  #'direction = #1
tieDown = \override Tie  #'direction = #-1
tieBoth = \revert Tie #'direction 

tieDotted = \override Tie  #'dashed = #1
tieSolid = \revert Tie #'dashed


dynamicUp = {
  \override DynamicText  #'direction = #1
  \override DynamicLineSpanner  #'direction = #1
}
dynamicDown = {
  \override DynamicText  #'direction = #-1
  \override DynamicLineSpanner  #'direction = #-1
}
dynamicBoth = {
  \revert DynamicText #'direction
  \revert DynamicLineSpanner #'direction
}

scriptUp = {
  \override TextScript  #'direction = #1
  \override Script  #'direction = #1
}
scriptDown = {
  \override TextScript  #'direction = #-1
  \override Script  #'direction = #-1
}
scriptBoth = {
  \revert TextScript #'direction
  \revert Script #'direction
}

dotsUp = \override Dots  #'direction = #1
dotsDown = \override Dots  #'direction = #-1
dotsBoth = \revert Dots #'direction 

tupletUp = \override TupletBracket  #'direction = #1
tupletDown = \override TupletBracket  #'direction = #-1
tupletBoth = \revert TupletBracket #'direction

cadenzaOn = \set Timing.timing = ##f
cadenzaOff = {
  \set Timing.timing = ##t
  \set Timing.measurePosition = #(ly:make-moment 0 1)
}

newpage = \notes
{
  \break
  % urg, only works for TeX output
  \context Score \applyoutput
  #(outputproperty-compatibility (make-type-checker 'paper-column-interface)
    'between-system-string "\\newpage")
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


% End the incipit and print a ``normal line start''.
endincipit = \notes \context Staff {
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



% To remove a Volta bracket or some other graphical object,
% set it to turnOff. Example: \set Staff.VoltaBracket = \turnOff

%%
%% DO NOT USE THIS. IT CAN LEAD TO CRASHES.
turnOff = #(cons '() '())

% For drawing vertical chord brackets with \arpeggio
% This is a shorthand for the value of the print-function property 
% of either Staff.Arpeggio or PianoStaff.Arpeggio, depending whether 
% cross-staff brackets are desired. 

arpeggioBracket = #Arpeggio::brew_chord_bracket
arpeggio = #(make-music 'ArpeggioEvent)
glissando = #(make-music 'GlissandoEvent)

fermataMarkup = \markup { \musicglyph #"scripts-ufermata" } 

setMmRestFermata =
  \once \override MultiMeasureRestNumber  #'text =
    #fermataMarkup 


hideNotes =\sequential {
				% hide notes, accidentals, etc.
    \override Dots  #'transparent = ##t
    \override NoteHead  #'transparent = ##t
    \override Stem  #'transparent = ##t
    \override Beam  #'transparent = ##t
    \override Staff.Accidental  #'transparent = ##t
}


unHideNotes = \sequential {
  \revert Staff.Accidental #'transparent
  \revert Beam #'transparent
  \revert Stem #'transparent
  \revert NoteHead #'transparent
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
