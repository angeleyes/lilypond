\version "2.4.0"
\header {

    texidoc = "@cindex Key Signature End of Line
According to normal typesetting conventions, LilyPond typesets key
changes at the end of the line, when the change appears at a line break.
This example shows how to change this default to only print the
new key signature at the beginning of the next line.
" }

\score {
   \relative c' {
    \set Staff.explicitKeySignatureVisibility = #end-of-line-invisible
    c d e f | g a b c | \break
    \key d \major
    d cis b a | g fis e d |
  }
}
