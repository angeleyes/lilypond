\header{
filename =	 "scales.ly";
enteredby =	 "HWN";
copyright =	 "public domain";
}

%{
 Tested Features: scripts, beams, transposition, 
%}

%
% scales with accents.
%

\version "0.1.8";
blah = 	\melodic {
		\meter 6/8;	
		\octave  'c ;
		\clef "bass";
			c4. d
		\octave  c ;
			c d 
		\clef "violin" ;
		\octave  c' ;
		\transpose  d { e f } 
		\octave  c'' ;
		c4. d 
	%ugr
		 |[6/9 a8 \< a a a a a a  \! a a \ff \> ]1/1 |
		\octave  c' ;
		 |[6/9 a  a a a a a a a \! a ]1/1 |
		\octave  c'' ; 
	
		 [6/9'c->-.-\fermata \< 'g->-.-\fermata 
			d->-.-\fermata a->-.-\fermata
			e'->-.-\fermata b'-. f''-. c'''->-.-\fermata 
			\! g'''->-.-\fermata \> ]1/1

		[6/9 g'''->-.-\fermata c'''->-.-\fermata
			f''->-.-\fermata b'->-.-\fermata e'->-.-\fermata 
			a->-.-\fermata d->-.-\fermata 'g->-.-\fermata \! 
			'c->-.-\fermata ]1/1
		 \octave c;
		[6/9'c->-.-\fermata \< 'g->-.-\fermata d->-.-\fermata 
		a->-.-\fermata
			e'->-.-\fermata b'-. f''-. 
			c'''->-.-\fermata \! g'''->-.-\fermata \> ]1/1
		 [6/9 g'''->-.-\fermata c'''->-.-\fermata
			f''->-.-\fermata b'->-.-\fermata e'->-.-\fermata 
			a->-.-\fermata d->-.-\fermata 
			'g->-.-\fermata \! 'c->-.-\fermata ]1/1
		\octave  c' ;
		\octave  c' ;   
		 [2/3 c g d' ]1/1 
		 [2/3 d' g c ]1/1  
		 [2/3 f c' g' ]1/1 
		 [2/3 g' c' f ]1/1  
		\octave  c ;
		 [2/3 c g d' ]1/1 
		 [2/3 d' g c ]1/1  
		 [2/3 f c' g' ]1/1 
		 [2/3 g' c' f ]1/1 
		 [2/3 g' c' f ]1/1 
  \meter 4/4;

c1
	\duration  8;
		r8-"text" r8^. r8_. r8 r8 r8 r8 r8
	[c-> d-> e-> f->][g-> a-> b-> c'->] % 1
\octave c';	[c'-^ b-^ a-^ g-^][f-^ e-^ d-^ c-^]
\octave c;	[c'-^ b-^ a-^ g-^][f-^ e-^ d-^ c-^]
	[c-. d-. e-. f-.][g-. a-. b-. c'-.]
	[c'-- b-- a-- g][f-- e-- d c--] % 5
	[c-\portato d-\portato e-\portato f-\portato]
		[g-\portato a-\portato b-\portato c'-\portato]
	[c'-\upbow b-\upbow a-\downbow g-\downbow]
		[f-\downbow e-\downbow d-\upbow c-\upbow]
	[c-| d-| e-| f-|][g-| a-| b-| c'-|]
	[c' b a g][f e d c]
	[c d e f][g a b c'] % 10 
	|[c' b a g][f e d c]
			
	}


\score{
	\melodic {\blah}
	\paper{
	}
}
