\version "1.0.20";

FontBody=	\notes\transpose c''{ 
		\bar "|:";
		\time 4/4;
		\context Staff  < \context Voice = VB { \stemup e'\longa a\breve | }
		\context Voice = VA { \stemdown
		c1 \clef "bass"; b,, \clef "violin"; c' a'
		c2 \clef "alto"; g c' \clef "violin"; a'
		} >
		\stemboth
		c4 g c' a' \bar ":|";
		a\ppp-\upbow a\pp-\downbow a\p^\turn a\mp^\fermata |
		a\mf_\fermata a\f-\stopped a\ff-\open a\fff^\trill|
		a\fp-\reverseturn a4.\sf a4.\sfz |  a4\fz % a\rf
		[c8-\prall c-\mordent] [a'-\prallmordent a'-\prallprall]
		[c8-\upprall a'8-\downprall] [a' c] |
		[c \< d e f] [as' ges' f' e']
		[cis' dis' c' des'] [cisis' disis' \! ceses' deses'] |
		\clef "bass";
		  r\longa * 1/4 r\breve *1/2
		  r1 r2 r4 r8 r16 r32 r64 r128 r128 |
		\context Staff < \context Voice = VA { \stemup r2 c'2 c,,,1 }
				\context Voice = VB {\stemdown r2 c2  r1 }>
			\stemboth
		\clef "violin";
		e8_. g'8-> e16^^ g'16_^ 
		e32 _| g'32^| g''32-\ltoe g''32-\lheel
		e64-\rtoe g'64-\rheel c4... |

		\transpose c'{
			\time 4/4;
			\property Voice . textstyle =  "finger"
			\property Voice . noteHeadStyle = "diamond"
			c1^"1" d2^"2" e4^"3"
			\property Voice . noteHeadStyle = "cross"
						 f4^"4"
			\property Voice . noteHeadStyle = "harmonic"
						  g4^"5"
			\property Voice . noteHeadStyle = ""
			% Music to the Martians!
			< bes4^"6" e_"7" c_"8" >
			a^"0"_"9"

			a'^\flageolet
			\property Voice . textstyle =  "roman"
			\time 1/2; a2 |
			\time 3/2; < a1.
			{ s4 \ppp \< s4 \! s4 \fff  s4 \> s4 \! s4\ppp} >
			|
			\time 2/4; a2 |
			\time 5/4; a1.. |
			\time 6/8; a2. |
			\time 7/8; a2.. |
			\time 9/8; a1... |
			\time 12/8; a1. |
			\time 12/4;
			r1. r2. r4. r8. r16. r32. r64. r64. |
		}
}
