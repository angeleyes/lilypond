% Toplevel initialisation file. 
	
\version "0.1.7";
  
breve = \duration { -1 0 }
longa = \duration { -2 0 }

\include "dynamic.ly"
\include "dutch.ly" 
\include "script.ly"


Gourlay = 1.0
Wordwrap = 0.0

\include "paper16.ly"
\include "paper20.ly"

default_paper = \paper{
	\paper_sixteen
}

% ugh
\include "midi.ly"

% declarations for standard directions
left = -1
right = 1
up = 1
down = -1
% zillie spellink?
center=0

\include "property.ly"

