
\header {

texidoc = "The flags of 8th notes take some space, but not
    too much: the space following a flag is less than the space
    following a beamed 8th head."

}

\score { \notes \relative c'' \context Staff {
    \property Voice.autoBeaming = ##f
    a8[ a8 a8 a8]
    a8 a8 a8 a8
}
\paper {
raggedright = ##t
    }
     }

\version "2.1.7"

