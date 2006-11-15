\version "2.10.0"
\sourcefilename "reverse-music.ly"

% possible rename to scheme-something.
\header { texidoc="@cindex Scheme Reverse Music
Symmetric, or palindromical music can be produced, first, by printing
some music, and second, by printing the same music applying a Scheme
function to reverse the syntax.
"
}

#(define (reverse-music music)
  (let* ((elements (ly:music-property music 'elements))
         (reversed (reverse elements))
         (e (ly:music-property music 'element))
         (span-dir (ly:music-property music 'span-direction)))

    (ly:music-set-property! music 'elements reversed)

    (if (ly:music? e)
        (ly:music-set-property!
         music 'element
         (reverse-music e)))

    (if (ly:dir? span-dir)
        (ly:music-set-property! music 'span-direction (- span-dir)))

    (map reverse-music reversed)

    music))

reverseMusic =
#(define-music-function (parser location m) (ly:music?)
		(reverse-music m)
	)	

music =  \relative c'' { c4 d4( e4 f4 }

\score {
\context Voice {
    \music
    \reverseMusic \music
  }
  \layout { ragged-right = ##t}
}

