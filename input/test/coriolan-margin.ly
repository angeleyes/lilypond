
\version "2.4.0"


% Ugh, we need to override some LaTeX titling stuff
\header {
  title =	"Ouvert�re"
  subtitle =	"Zu Heinrich Joseph v. Collins Trauerspiel"
  subsubtitle =	\markup { \large \bold "Coriolan" }
  opus = "Op. 62"
  piece = \markup { \large Allegro con brio }
  composer = 	 "Ludwig van Beethoven (1770-1827)"

texidoc = "@cindex Orchestra Score
@cindex Coriolan Score
In an orchestral score (Beethoven's Coriolan overture), there are 
different instrument groups, and some of the instruments may be
transposed. Instruments are indicated either with a long or short name.
" }

#(set-global-staff-size 16)

raisedFlat = \markup { \raise #0.4 \smaller \smaller \flat  }



flauti =  \relative c' {
  \set Staff.instrument = #"2 Flauti"
  \set Staff.instr = #"Fl."
  \time 4/4
  c1 
  \break c
%  \break c
%  \break c
  \bar"|."
}

oboi =  \relative c' {
  \set Staff.instrument = #"2 Oboi"
  \set Staff.instr = #"Ob."
  c1 c
}

clarinetti =  \relative c' {
    \set Staff.instrument = \markup { \column < "Clarinetti" { "in B" \raisedFlat } > }
    \set Staff.instr = \markup { \smaller  { "Cl(B" \raisedFlat ")" } }

  c1 c
}

fagotti =  \relative c' {
  \set Staff.instrument = #"2 Fagotti"
  \set Staff.instr = #"Fg."
  c1 c
}

corni =  \relative c' {
    \set Staff.instrument = \markup { \column < "Corni" { "in E" \raisedFlat } > }
    \set Staff.instr = \markup { \smaller  { "Cor(E" \raisedFlat ")" } }

  c1 c
}

trombe =  \relative c' {
  \set Staff.instrument = \markup \column < "2 Trombe" "(C)" >
  \set Staff.instr = \markup \column <  "Tbe." "(C)" >

  c1 c
}

timpani =  \relative c' {
  \set Staff.instrument = \markup \column < "Timpani" "(C-G)" >
  \set Staff.instr = #"Timp."

  c1 c
}

violinoI =  \relative c' {
  \set Staff.instrument = #"Violino I  "
  \set Staff.instr = #"Vl. I  "
  c1 c
}

violinoII =  \relative c' {
  \set Staff.instrument = #"Violino II  "
  \set Staff.instr = #"Vl. II  "
  c1 c
}

viola =  \relative c' {
  \set Staff.instrument = #"Viola"
  \set Staff.instr = #"Vla."
  c1 c 
  %c
}

violoncello =  \relative c' {
  \set Staff.instrument = \markup \column < "Violoncello" "e" "Contrabasso" >
  \set Staff.instr = \markup \column <  "Vc." "Cb." >
  c1 c
}


\paper {
    indent=10.0\mm
    linewidth=150.0\mm
    raggedbottom = ##t 
}

#(set-global-staff-size 16)
\book {
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
	    \context Staff ="violinoI" \violinoI
	    \context Staff ="violinoII" \violinoII
	  >>
	  \context Staff ="viola" \viola
	  \context Staff ="violoncello" \violoncello
	>>
      >>
      \layout {
	  \context {
	      \RemoveEmptyStaffContext
	  }
	  \context {
	      \Score
	      \override TimeSignature #'style = #'C
	  }
      }
    }
    % this is ignored?
    \paper {
	indent=10.0\mm
	linewidth=150.0\mm
    }
}
