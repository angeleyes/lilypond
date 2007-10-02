%
%  embosser-init.ly -- embosser context initialisations
%
%  source file of the GNU LilyPond music typesetter
%
%  (c) 2007 Ralph Little
%
\version "2.11.32"

\context {
	\name Global
	\accepts Score
	\description "Hard coded entry point for LilyPond. Cannot be tuned."
}

\context {
	\type "Score_embosser"
	\name Score
	myVariable = #'mysymbol
	\accepts Staff
	\defaultchild "Staff"
}

\context {
	\type "Embosser_group"
	\name Staff
	\accepts Voice
	\accepts CueVoice
	\defaultchild Voice
	\consists "Staff_embosser"
}

\context {
	\type "Embosser_group"
	\name Voice
	\consists "Note_embosser"
}
