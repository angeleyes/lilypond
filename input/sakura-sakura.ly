%% sakura-sakura.ly

\version "2.6.0"

\header {
  title = "さくら さくら"
  subtitle = "(Sakura, sakura)"
}


\score {
  
  <<
    \relative c'' {
      %% Beams are melismata, no autobeams.
      \set Staff.autoBeaming = ##f

      | a4 a b2 | a4 a b2
      | a4 b c b | a4 b8[ a] f2
      | e4 c e f | e4 e8[ c] b2
      | a'4 b c b | a4 b8[ a] f2
      | e4 c e f | e4 e8[ c] b2
      | a'4 a b2 | a4 a b2
      | d,4 e b'8[ a] f4 | e1 \bar "|."
    }
    \addlyrics {
      さ く ら さ く ら
      の や ま も さ と も
      み わ た す か ぎ り
      か す み か く も か
      あ さ ひ に に を う
      さ く ら さ く ら
      は な ざ か り
    }
  >>
  \layout { }
  \midi { \tempo 4=120 }
  }

%%% Local Variables:
%%% coding: utf-8
%%% End:
