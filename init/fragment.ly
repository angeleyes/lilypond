% Toplevel initialisation file. 
	
\version "1.0.10";


\include "declarations.ly"

\include "paper16.ly";

\paper { 
  \paper_sixteen
    linewidth = -1.0\cm;
    castingalgorithm = \Wordwrap;
    "unusedentry" = "}\\def\\nolilyfooter{";
}


\score { 
%  \notes\relative c {
  \notes {
    \maininput
  }
  \paper { }

}
