
\version "2.7.32"
\header {
    texidoc = "Beaming can be also given explicitly."
}

\layout { ragged-right= ##t }

\relative c'
{
  c16[ c8.]
  c8.[ c16]
  c8[ c16 c16 c8]
  c8[ c16 e16 g8]

}
