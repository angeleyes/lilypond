\header {

  texidoc = "With the @code{padding-pairs} property, distances
 between individual key signature items can be adjusted."

}

\version "2.12.0"

{
  \override Staff.KeySignature #'padding-pairs
    = #'((("accidentals.flat" . "accidentals.sharp.slashslash.stemstemstem") . 0.5))
    \set Staff.keySignature = #`((4 . ,FLAT) (6 . ,THREE-Q-SHARP) (2 . ,SEMI-FLAT))
  e2
}
