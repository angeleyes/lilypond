\header{
filename	 twinkle.ly
title	twinkle twinkle little star
description	traditional song in various languages.
	 "Ah, vous dirais-je, maman" 
	 "Altijd is Kortjakje ziek"
	 "Twinkle twinkle little star"

composers	Traditional
enteredby	HWN & JCN
copyright	public domain
}
%{
Tested Features: lyrics

%}
\version "0.1.1";

melody = \melodic{
	\clef violin;
	\octave c';
	c4 c | g g | a a | g g |
	f f | e e | d d8.( e16 | )c2 | % :|

	g4 g | f f | e e | d d |
	g g | f f | e( e8. f16 | e4 )d |

	c c | g g | a a | g g |
	f f | e e | d d8.( e16 | )c2 % :|
	\bar ":|";
}

accompany = \melodic {
	\clef "bass";
	\octave 'c;
	\duration 4;
	c4 c' | e' c' | f' c' | e' c' | 
	d' b | c' a | f g | c2 | 

	e'4 g | d g | c' g | b g | 
	e' g | d' g | c' c'8.( d'16 | c'4 )b |

	c c' | e' c' | f' c' | e' c' | 
	d' b | c' a | f g | c2 
	\bar ":|";
}

global = \melodic {
		\meter 2 / 4;
		\skip 2*24;
%		\bar "|.";
	}

tekst = \lyric{ 
 	Al-4 tijd is Kort- jak- je ziek,2
	midden4 in_de week maar s'_zon- dags niet.2
	s'_Zon-4 dags gaat ze naar de kerk,2
	met4 een boek vol zil- ver werk.2
	Al-4 tijd is Kort- jak- je ziek,2
	midden4 in_de week maar s'_zon- dags niet.2
}

%{

Ja inderdaad. Dit is geen educatieve danwel muzikaal verantwoorde
tekst. Mogen wij ook af en toe ergens op afgeven?

%}
hegedraagjetekst = \lyric{ 
 	Al-4 tijd zuigt Bill Gates mijn piek,2
	"\TeX"4 is slecht- ser dan mu- ziek.2
	s'_Zon-4 dags gaat het door een raam,2
	Weet4 dat ik me er- voor schaam.2
 	Al-4 tijd zuigt Bill Gates mijn piek,2
	"\TeX"4 is slecht- ser dan mu- ziek.2
}

texte = \lyric{ 
	 
	\textstyle "italic" ;
 	Ah!4 vous dir- ai_- je ma man2
	Ce4 qui cau- se mon tour- ment2
	Pa-4 pa veut que je rai- sonne2
	Comme4 un- e grand- e per- sonne2
	Moi4 je dis que les bon- bons2
	Val-4 ent mieux que la rai- son2
	
}

texti = \lyric{
	
	\textstyle "roman";
	Twin-4 kle, twin- kle, lit- tle star,2
	How4 I won- der what you are.2
	Up4 a- bove the world so high,2
	Like4 a dia- mond in the sky.2
	Twin-4 kle, twin- kle, lit- tle star,2
	How4 I won- der what you are!2
}

textii = \lyric{
	When4 the bla- zing sun is gone,2
	When4 he no- thing shines up- on,2
	Then4 you show your lit- tle light,2
	Twin-4 kle, twin- kle, all the night.2
	Twin-4 kle, twin- kle, lit- tle star,2
	How4 I won- der what you are!2
	
}

textiii = \lyric{
	
	Then4 the tra- veler in the dark2
	Thanks4 you for your ti- ny spark;2
	He_could4 not see which way to go,2
	If4 you did not twin- kle so.2
	Twin-4 kle, twin- kle, lit- tle star,2
	How4 I won- der what you are!2
	
}

\score{
	< 
		\melodic <
			\id "Piano" "";\multi 2;
			< \global 
				\melody >
			< \global 
				\accompany >
		>
		
		% ugh
		\lyric < \id "Lyric" "1"; 
			\global \tekst >
		\lyric < \id "Lyric" "2";  \global
			\texte >
		\lyric < \id "Lyric" "3"; \global
			\texti \textii \textiii >
		
	>
	\paper{
	}
	\midi{ 
		\tempo 4 = 120 ;
	}
}

