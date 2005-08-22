\version "2.6.0"
\header {

  texidoc ="Polyphonic rhythms and rests do not disturb
@code{\lyricsto}."

}

\layout { raggedright = ##t}

{
  \clef violin
  \time 8/8
  \key des \major
  <<
    \lyricsto "one"  \new Lyrics  {
      Do mi nus ex
    }
    \context Voice = "one" {
      \voiceOne
      bes'4 bes'4
      bes'4 bes'4
    }
    \context Voice = "two" {
      \voiceTwo
      ees'8 r8 r8 r8 ees' r8 r8 r8 
    }
    \lyricsto "two"  \new Lyrics  {
      Do na
    }
  >>
}



