#(ly:set-option 'old-relative)
\version "1.9.4"
\header{
filename = 	 "twinkle-pop.ly"
xtitle = 	 "Ah, vous dirais-je, maman "
composer = 	 "traditional"
enteredby = 	 "HWN, chords by Johan Vromans"
copyright = 	 "public domain"
}


melodie =  \notes\relative c'' {
        \clef "violin"
        \time 2/4 

        c4 c  | g' g | a a | g2  |
        f4 f  | e e  | d d | c2  |
        g'4 g | f f  | e e | d d |
        g  g  | f f  | e e | d d |
	% copy 1-8
        c4 c  | g' g | a a | g2  |
        f4 f  | e e  | d d | c2 \bar "|."
}

acc =  \chords {
	% why don't \skip, s4 work?
        c2 c f c
        f c g:7 c
	g f c  g:7 % urg, bug!
        g f c  g:7
	% copy 1-8
        c2 c f c
        f c g:7 c
}

text =  \lyrics{ 
        \property Lyrics . LyricText \set #'font-shape = #'italic

        Ah!4 vous dir -- ai -- je ma man2
        Ce4 qui cau -- se mon tour -- ment2
        Pa4 --  pa veut que je rai -- so -- nne
        Comme4 un -- e grand -- e per -- so -- nne
        Moi4 je dis que les bon -- bons2
        Val4 -- ent mieux que la rai -- son2
        
}

\score {
        <<  
	   \context ChordNames \acc
	   \context Staff=melody \melodie
           \context Lyrics \text
        >>
	\header{
		title = "Ah, vous dirais-je, maman "
	}
        \paper {  }
}

\score {
        <<  
	   \chords \context ChordNames \transpose c d\acc
	   \notes \context Staff=melody \transpose c d\melodie
           \lyrics \context Lyrics \text
        >>
	\header{
		piece = "clarinet in B$\flat$"
	}
        \paper {  }
}

