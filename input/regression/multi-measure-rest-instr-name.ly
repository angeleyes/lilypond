\version "1.9.0"
\header { texidoc = "This combines instrument names and multimeasure rests (there was an
interesting bug in 1.3.98). " }

\score{
	\context Staff <
	      \property Staff.instrument = "instrument"
	      \property Staff.instr = "instr"
	      \notes{c''1 \break R1 }
	    >
	 \paper {
	    \translator { \StaffContext
	    \consists Instrument_name_engraver

	    }
	  }
	\paper { raggedright =  ##t }
	}


