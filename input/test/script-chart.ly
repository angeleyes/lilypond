\version "2.1.22"

% this chart is used in the manual too.

\header {
    texidoc ="@cindex Feta scripts

A chart showing all feta scripts.

"
}

\score {
    <<
      \override Score.LyricText  #'font-family = #'typewriter
      \override Score.LyricText  #'font-shape = #'upright
      \context Staff \notes {
	\set Score.timing = ##f
	\set Score.barAlways = ##t
	\override Staff.BarLine  #'transparent = ##t
        c''\accent             c''\marcato            c''\staccatissimo
        c''\staccato           c''\tenuto             c''\portato
        c''\upbow              c''\downbow            c''\flageolet
        c''\thumb              c''^\lheel              c''\rheel
        c''^\ltoe               c''\rtoe               c''\open
        c''\stopped            c''\turn               c''\reverseturn
        c''\trill              c''\prall              c''\mordent
        c''\prallprall         c''\prallmordent       c''\upprall
        c''\downprall          c''\upmordent          c''\downmordent
        c''\pralldown          c''\prallup            c''\lineprall
        c''\signumcongruentiae c''\shortfermata       c''\fermata
        c''\longfermata        c''\verylongfermata    c''\segno
        c''\coda               c''\varcoda
      }
      \context Lyrics \lyrics {
        accent__                marcato__               staccatissimo__
        staccato__              tenuto__                portato__
        upbow__                 downbow__               flageolet__
        thumb__                 lheel__                 rheel__
        ltoe__                  rtoe__                  open__
        stopped__               turn__                  reverseturn__
        trill__                 prall__                 mordent__
        prallprall__            prallmordent__          upprall__
        downprall__             upmordent__             downmordent__
        pralldown__             prallup__               lineprall__
        signumcongruentiae__    shortfermata__          fermata__
        longfermata__           verylongfermata__       segno__
        coda__                  varcoda__
      }
    >>
    \paper {
      linewidth = 5.1\in
      indent = 0.0\mm
    }
  }


