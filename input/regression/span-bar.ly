\version "2.2.0"
\header {
  texidoc = "Span bars are drawn only between staff bar lines. By setting 
bar lines to transparent, they are shown only between systems.

Setting @code{SpanBar} transparent does the removes the barlines
between systems.
 
"
}

\score {
  \notes \relative c' \new StaffGroup <<
    \new Staff {
      \override Score.BarLine #'transparent = ##t
      a1 a1
      \revert Score.BarLine #'transparent
      \override Score.SpanBar #'transparent = ##t
      a1 a1
    }
    \lyrics <<
      \new Lyrics { bla1 die bla }
      \new Lyrics { foo bar foo }
    >>
    \new Staff {
      f1 f1 f1 f1
    }
  >>
  \paper {
    \context {
      \StaffContext
    }
    raggedright =##t 
  }
}


%% Local variables:
%% LilyPond-indent-level:2
%% End:

