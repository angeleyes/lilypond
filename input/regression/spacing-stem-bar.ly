
\version "2.1.36"
\header {

texidoc = "Upstem notes before a barline are printed with some extra
space. This is an optical correction similar to juxtaposed stems.
"

}

sd = \override Stem  #'direction = #-1
su = \override Stem  #'direction = #1
\score { \notes\relative e'
{

%\override Staff.StaffSpacing  #'stem-spacing-correction = #0.5
%\override Staff.NoteSpacing  #'stem-spacing-correction = #0.5

\time 3/8
\su
e8 e e
f f f
a a a
c c c
e e e
}
\paper { raggedright = ##t}
}

