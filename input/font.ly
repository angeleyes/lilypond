\header{
title= "The Feta font";
subtitle = "proofsheet"; 
enteredby =	 "jcn";
copyright =	 "public domain";
TestedFeatures =	 "This file tests the Feta music-font"
	 "(Feta definitively is not an abbreviation of Font-En-TjA)";
}

\version "0.1.7";

\score{
	\melodic{ 
		\octave c';
		\bar "|:";
		\meter 4/4;
		\multi 2  < { \stemup e'\longa a\breve | }
		{ \stemdown
		c1 \clef "bass"; ''b \clef "violin"; c' a'
		c2 \clef "alto"; g c' \clef "violin"; a'
		} >
		\stemboth
		c4 g c' a' \bar ":|";
		a\ppp-\upbow a\pp-\downbow a\p^\turn a\mp^\fermata |
		a\mf_\fermata a\f-\stopped a\ff-\open a\fff^\trill|
		a\fp a4.\sf a4.\sfz | % a\fz a\rf
		[c8 c] [a' a']
		[c a'] [a' c] |
		[c d e f] [as' ges' f' e']
		[cis' dis' c' des'] [cisis' disis' ceses' deses'] |
		\clef "bass";
		  r1 r2 r4 r8 r16 r32 r64 r128 r128 |
		\multi 2 < { \stemup r2 r2} {\stemdown c c }>
		\multi 2 < { \stemup ''c1 } {\stemdown r1}>		
		\stemboth
		\clef "violin";
		e8_. g'8-> e16^^ g'16_^ 
		e32 _| g'32^| g''32-\ltoe g''32-\lheel
		e64 g'64 c4... |

		\octave c'';
		\meter 4/4;
		\textstyle "finger";
		c4^"1" d^"2" e^"3" f^"4" g^"5"
		< bes4^"1" e_"2" c_"4" >
		a'^"0"
		
		% ugh, \flageolet is like open, but maybe a perfect circle
		a^\open
		\textstyle "roman";
%{		% default abbreviations
		c4 c4: c4:32 c4: c4 c2.
		'a1
		'a1:32
		'c4:8 c': 'c4:16 c':
%		[ 'c8:16 'c 'c 'c ] [ a a a a ]
%		[ 'c 'f 'b e ] 
%		[ 'c16:32 'c 'c 'c ] [ a16:32 a a a ]
%%		% there's still some hairy beam bugfixing todo
%%		[ c'8:16 g d 'a ]
%%		[ c'16:32 g d 'a ]
%%		[ 'c8:32 'f 'b e ]
		[:32 c16 e]
		[:32 c16 e]
		[:16 c8 e]
		[:16 e4 g]
		[:16 e2 g]
		[:16 e1 g]
%}
		\meter 1/2; a2 |
		\meter 3/2; a1. |
		\meter 2/4; a2 |
		\meter 5/4; a1.. |
		\meter 6/8; a2. |
		\meter 7/8; a2.. |
		\meter 9/8; a1... |
		\meter 12/8; a1. |
		\meter 12/4;
		r1. r2. r4. r8. r16. r32. r64. r64. |
		}
	\paper{ 
	    % don't change this.
	    % otherwise 16pt and 20pt layouts differ.
	    linewidth = 12.5 \cm;
	    gourlay_maxmeasures =5.;
	}
	\paper{
	    \paper_twenty
	    linewidth = 17.5 \cm;
	    gourlay_maxmeasures =5.;
	    \output "font20.tex";
	}
}

