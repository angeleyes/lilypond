\version "1.7.18"
\header { texidoc = "@cindex Incipit
This shows how to make an ``incipit'' to indicate scordatora 
tuning of a violin part, using the clefStyle property.
The two first bars of Biber's Rosary sonata III. " }

violinincipit =  \notes\relative c''{
  \clef "french"
  \time 2/2
  \property Staff.TimeSignature \override #'style = #'old
  a4. b8 c4 fis |
%  <<b fis' b d>>1
  \property Staff.TimeSignature \override #'style = #'C
}

bcincipit =  \notes\relative c{
  \clef bass
  \property Staff.TimeSignature \override #'style = #'old
  b2. cis4 | 
  \property Staff.TimeSignature \override #'style = #'C
}

violin =  \notes\relative c''{
% Key signatures with different alterations in different octaves
% are broken since 1.3.58!
%  \specialkey \keysignature f' fis'' g' gis''
  \key d \major
  \time 2/2
  \clef treble

  a4. b8 c4 fis |
  gis~ gis8 fis16^\trill (e-) b8 c \context Staff<{\voiceOne a d}{\voiceTwo es,4}>|
}

BC  = \notes\relative c{
  \key d \major
  \time 2/2
  \clef "bass"

 \key \default
  b2. cis4 | 
  d e fis g |
}

\score{
  <
    \context Staff = violin {\notes{
      \property Staff.Clef \override #'transparent = ##t
      \violinincipit \bar ".|" 
      \property Staff.Clef \revert #'transparent 
      \endincipit
      \violin
    }}
    \context Staff = BC{\notes{
      \property Staff.Clef \override #'transparent = ##t
      \bcincipit \bar ".|" 
      \property Staff.Clef \revert #'transparent 
      \endincipit
      \BC
    }}
  >
	\paper { raggedright = ##t }
}  


