
\header {
texidoc = "The full orchestra plays a notes, where groups stop one after another. USe this to tune equalizer settings. "
}
\version "1.7.6"

flauti = \notes \relative c' {
  \property Staff.midiInstrument = #"flute"
  \property Staff.instrument	= #"2 Flauti"
  \property Staff.instr		= #"Fl."

  c1 R1*10
}

oboi = \notes \relative c' {
  \property Staff.midiInstrument = #"oboe"
  \property Staff.instrument	= #"2 Oboi"
  \property Staff.instr		= #"Ob."

  R1*1 c1 R1*9
}

clarinetti = \notes \relative c' {
  \property Staff.midiInstrument = #"clarinet"
  \property Staff.instrument	= #`(lines "2 Clarinetti" (columns "(B" (music "accidentals--1") ")"))
  \property Staff.instr		= #`(lines "Cl."(columns "(B" (music "accidentals--1") ")"))

  R1*2 c1 R1*8
}

fagotti = \notes \relative c' {
  \property Staff.midiInstrument = #"bassoon"
  \property Staff.instrument	= #"2 Fagotti"
  \property Staff.instr		= #"Fg."

  \clef bass
  R1*3 c1 R1*7
}

corni = \notes \relative c' {
  \property Staff.midiInstrument = #"french horn"
  \property Staff.instrument	= #`(lines "2 Corni" (columns "(E" (music "accidentals--1") ")"))
  \property Staff.instr		= #`(lines "Cor." (columns "(E" (music "accidentals--1") ")"))

  R1*4 c1 R1*6
}

trombe = \notes \relative c' {
  \property Staff.midiInstrument = #"trumpet"
  \property Staff.instrument	= #'(lines "2 Trombe" "(C)")
  \property Staff.instr		= #'(lines "Tbe." "(C)")

  \clef bass
  R1*5 c1 R1*5
}

timpani = \notes \relative c' {
  \property Staff.midiInstrument = #"timpani"
  \property Staff.instrument	= #'(lines "Timpani" "(C-G)")
  \property Staff.instr		= #"Timp."

  R1*6 c1 R1*4
}

violinoI = \notes \relative c' {
  \property Staff.midiInstrument = #"violin"
  \property Staff.instrument	= #"Violino I "
  \property Staff.instr		= #"Vl. I "

  R1*7 c1 R1*3
}

violinoII = \notes \relative c' {
  \property Staff.midiInstrument = #"violin"
  \property Staff.instrument	= #"Violino II "
  \property Staff.instr		= #"Vl. II "
 
  R1*8 c1 R1*2
}

viola = \notes \relative c' {
  \property Staff.midiInstrument = #"viola"
  \property Staff.instrument	= #"Viola"
  \property Staff.instr		= #"Vla."

  \clef alto
  R1*9 c1 R1*1
}

violoncello = \notes \relative c' {
  \property Staff.midiInstrument = #"cello"
  %\property Staff.midiInstrument = #"contrabass"
  \property Staff.instrument	= #'(lines "Violoncello" "e" "Contrabasso")
  \property Staff.instr		= #'(lines "Vc." "Cb.")
  
  \clef bass
  R1*10 c1
}

\include "paper16.ly"

\score {
  < 
    \context StaffGroup ="legni" < 
      \context Staff ="flauti" \flauti
      \context Staff ="oboi" \oboi
      \context Staff ="clarinetti" \clarinetti 
      \context Staff ="fagotti" \fagotti 
    >
    \context StaffGroup ="ottoni" <
      \context Staff ="corni" \corni
      \context Staff ="trombe" \trombe
    >
    \context StaffGroup ="timpani" <
      \context Staff ="timpani" \timpani
     { 
       \skip 1 
       % Hmm: this forces a staff-bracket, that's good!
       % However, I can't find where is decided on staff-bracket yes/no
     }
    >
    \context StaffGroup ="archi" <
      \context GrandStaff ="violini" <
        \context Staff ="violino1" \violinoI
        \context Staff ="violino2" \violinoII
      >
      \context Staff ="viola" \viola
      \context Staff ="violoncello" \violoncello
    >
  >
 \header{
		title = "Coriolan"
		subtitle = "Ouverture" 
		opus = "Opus 62"
		composer = "Ludwig van Beethoven (1770-1827)"
		enteredby = "JCN"
		copyright = "public domain"
	}

  \paper {
  	\paperSixteen
  	indent=100.0\mm
  	linewidth=150.0\mm
    \translator {
      \HaraKiriStaffContext
    }
  }
  \midi {
  	\tempo 1 = 60
  }
}

%% new-chords-done %%
