#(ly:set-option 'old-relative)
\version "2.1.29"
\header {
texidoc = "@cindex Midi Volume Equaliser
The full orchestra plays a notes, where groups stop one after
another. Use this to tune equalizer settings. "
}

#(set-global-staff-size 16)

%{

Override, see scm/midi.scm:

#(set! instrument-equalizer-alist
      (append 
       '(
	 ("flute" . (0 . 0.7))
        )
      instrument-equalizer-alist))

%}

flauti = \notes \relative c' {
  \set Staff.midiInstrument = #"flute"
  \set Staff.instrument = #"2 Flauti"
  \set Staff.instr = #"Fl."

  c1\f R1*10
}

oboi = \notes \relative c' {
  \set Staff.midiInstrument = #"oboe"
  \set Staff.instrument = #"2 Oboi"
  \set Staff.instr = #"Ob."

  R1*1 c1\f R1*9
}

clarinetti = \notes \relative c' {
  \set Staff.midiInstrument = #"clarinet"
  \set Staff.instrument = #"Clarinetti"
  \set Staff.instr = #"Cl"

  R1*2 c1\f R1*8
}

fagotti = \notes \relative c' {
  \set Staff.midiInstrument = #"bassoon"
  \set Staff.instrument = #"2 Fagotti"
  \set Staff.instr = #"Fg."

  \clef bass
  R1*3 c1\f R1*7
}

corni = \notes \relative c' {
  \set Staff.midiInstrument = #"french horn"
  \set Staff.instrument = #"Corni"
  \set Staff.instr = #"Cor"

  R1*4 c1\f R1*6
}

trombe = \notes \relative c' {
  \set Staff.midiInstrument = #"trumpet"
  \set Staff.instrument = #"Trombe"
  \set Staff.instr = #"Tp."

  \clef bass
  R1*5 c1\f R1*5
}

timpani = \notes \relative c' {
  \set Staff.midiInstrument = #"timpani"
  \set Staff.instrument = #"Timpani"
  \set Staff.instr = #"Timp."

  R1*6 c1\f R1*4
}

violinoI = \notes \relative c' {
  \set Staff.midiInstrument = #"violin"
  \set Staff.instrument = #"Violino I "
  \set Staff.instr = #"Vl. I "

  R1*7 c1\f R1*3
}

violinoII = \notes \relative c' {
  \set Staff.midiInstrument = #"violin"
  \set Staff.instrument = #"Violino II "
  \set Staff.instr = #"Vl. II "
 
  R1*8 c1\f R1*2
}

viola = \notes \relative c' {
  \set Staff.midiInstrument = #"viola"
  \set Staff.instrument = #"Viola"
  \set Staff.instr = #"Vla."

  \clef alto
  R1*9 c1\f R1*1
}

violoncello = \notes \relative c' {
  \set Staff.midiInstrument = #"cello"
  %\set Staff.midiInstrument = #"contrabass"
  \set Staff.instrument = #"Violoncello"
  \set Staff.instr = #"Vc."
  
  \clef bass
  R1*10 c1\f
}


\score {
  << 
    \context StaffGroup ="legni" << 
      \context Staff ="flauti" \flauti
      \context Staff ="oboi" \oboi
      \context Staff ="clarinetti" \clarinetti 
      \context Staff ="fagotti" \fagotti 
    >>
    \context StaffGroup ="ottoni" <<
      \context Staff ="corni" \corni
      \context Staff ="trombe" \trombe
    >>
    \context StaffGroup ="timpani" <<
      \context Staff ="timpani" \timpani
     { 
       \skip 1 
       % Hmm: this forces a staff-bracket, that's good!
       % However, I can't find where is decided on staff-bracket yes/no
     }
    >>
    \context StaffGroup ="archi" <<
      \context GrandStaff ="violini" <<
        \context Staff ="violino1" \violinoI
        \context Staff ="violino2" \violinoII
      >>
      \context Staff ="viola" \viola
      \context Staff ="violoncello" \violoncello
    >>
  >>

  \paper {
  	\paperSixteen
  	indent=100.0\mm
  	linewidth=150.0\mm
    \context {
      \RemoveEmptyStaffContext
    }
  }
  \midi {
  	\tempo 1 = 60
  }
}


