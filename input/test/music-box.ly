\version "2.1.23"
\include "deutsch.ly"
% possible rename to scheme- something.  -gp
% TODO: ask if it needs to have so many empty bars in the middle.  -gp
\header{ texidoc = "@cindex Scheme Music Box
This example shows prelude in C major of WTK1, but coded
using Scheme functions to avoid typing work. " }


#(define (transform music)
  (let* ((es (ly:get-mus-property music 'elements))
         (n  (ly:music-name music))
        )
   (if (not (equal? n "Sequential_music"))
     (ly:warn "transform needs sequential music!")
     (begin
      (let recurse ((elts es))
       (if (not (equal? elts '()))
	 (begin
           ((trans (ly:get-mus-property (cadr elts) 'elements)) (car elts))
	   (set-cdr! elts (cddr elts))
	   (recurse (cdr elts))
	 )
       )
      )
     )
   )
   music
  )
 )

#(define ((trans pitches) music)
  (let* ((es (ly:get-mus-property music 'elements))
         (e (ly:get-mus-property music 'element))
         (p (ly:get-mus-property music 'pitch)))

    (if (pair? es)
        (ly:set-mus-property!
         music 'elements
         (map (trans pitches) es)))

    (if (ly:music? e)
        (ly:set-mus-property!
         music 'element
         ((trans pitches) e)))

    (if (ly:pitch? p)
       (let* ((o (ly:pitch-octave p))
              (n (ly:pitch-notename p))
              (i (+ (* 7 o) n))
	      (pes (ly:get-mus-property (list-ref pitches i) 'elements))
	      (pnew (ly:get-mus-property (car pes) 'pitch))
             )
          (ly:set-mus-property! music 'pitch pnew)
	)
    )
    music
  )
 )





\version "2.1.23"

pat = \notes \transpose c c' \repeat unfold 2 {
  << { \context Staff=up {r8 e16 f g e f g } }
    { \context Staff=down <<
      \context Voice=vup  { \stemUp \tieUp r16 d8.~d4 }
      \context Voice=vdwn { \stemDown  c2 }
    >> }
  >>
}

enda = \notes { r8 f,16 a, c f c a, \stemUp c \change Staff = down
      a, f, a, f, d, f, d, \change Staff = up \stemBoth
      r8 g16 h d' f' d' h d' h g h d f e\prall d <e g c'>1^\fermata \bar "|."
}
endb = \notes {\stemUp \tieUp r16 c,8.~c,4~c,2 r16 h,,8.~h,,4~h,,2 c,1 \bar "|."}
endc = \notes {\stemDown \tieDown c,,2~c,, c,,~c,, c,,1_\fermata }

\score {
  \notes \transpose c c' \context PianoStaff <<
    \context Staff=up   { \clef "G"  }
    \context Staff=down { \clef "F" }
    { \apply #transform {\pat {c e g c' e' }
                         \pat {c d a d' f' }
			 \pat {h, d g d' f' }
			 \pat {c e g c' e' }
		     }
      
      %{
      %Etc.

      %we get the idea now.


      
			 \pat {c e a e' a' }
			 \pat {c d fis a d'  }
			 \pat {h, d g d' g' }
			 \pat {h, c e g c' }
			 \pat {a, c e g c'  }
			 \pat {d, a, d fis c' }
			 \pat {g, h, d g h }
			 \pat {g, b, e g cis'  }
			 \pat {f, a, d a d' }
			 \pat {f, as, d f h }
			 \pat {e, g, c g c' }
			 \pat {e, f, a, c f }
			 \pat {d, f, a, c f }

			 \pat {g,, d, g, h, f }
			 \pat {c, e, g, c e }
			 \pat {c, g, b, c e }
			 \pat {f,, f, a, c e  }
			 \pat {fis,, c, a, c es }
			 \pat {as,, f, h, c d }
			 \pat {g,, f, g, h, d }
			 \pat {g,, e, g, c e }
			 \pat {g,, d, g, c f }
			 \pat {g,, d, g, h, f }
			 \pat {g,, es, a, c fis }
			 \pat {g,, e, g, c g }
			 \pat {g,, d, g, c f }
			 \pat {g,, d, g, h, f }
			 \pat {c,, c, g, b, e }
			%}
    }
  >>
  \paper {
    \translator {
      \PianoStaffContext
      \override VerticalAlignment #'forced-distance = #10
    }

    linewidth = 18.0 \cm
  }
  \midi {
      \tempo 4 = 80
  }
}

