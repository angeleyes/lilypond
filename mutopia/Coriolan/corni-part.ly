\header{
filename =	 "corni-part.ly";
% %title =	 "Ouvert\\"ure zu Collins Trauerspiel \\"Coriolan\\" Opus 62";
description =	 "";
composer =	 "Ludwig van Beethoven (1770-1827)";
enteredby =	 "JCN";
copyright =	 "public domain";
}

\version "1.3.4";

\include "global.ly"
\include "corni.ly"
\score{
	\$corni_staff
	\include "coriolan-part-paper.ly"
	\midi{ \tempo 4 = 160; }
}


