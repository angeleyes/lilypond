
\version "2.3.22"
% MERGE with title.ly -gp

%{
  Test lilypond features.
%}

\header {
copyright = "copyright"
title = "title"
subtitle = "subtitle"
composer = "composer"
arranger = "arranger"
instrument = "instrument"
metre = "metre"
opus = "opus"
piece = "piece"
poet = "poet"
texidoc = "All header fields with special meanings."
  copyright = "public domain"
  enteredby = "jcn"
  source = "urtext"

}


\score {
  \relative c'' { c1 c1 c1 c1 }
}

\score {
	 \relative c'' { c1 c1 c1 c1 }
	
	\header {

	title = "localtitle"
	subtitle = "localsubtitle"
	composer = "localcomposer"
	arranger = "localarranger"
	instrument = "localinstrument"
	metre = "localmetre"
	opus = "localopus"
	piece = "localpiece"
	poet = "localpoet"
	copyright = "localcopyright"
	}
}

