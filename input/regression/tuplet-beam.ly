
\version "1.9.1"
\header {

texidoc = "In combination with a beam, the bracket of the tuplet
bracket is removed. This only happens if there is one beam, as long as
the bracket."

}
    \paper { raggedright= ##t }

\score { \notes \context Voice\relative c'' {
\times 2/3 { r  c8[ c8] }
\times 2/3 {  c8[ c c] }
\times 2/3 {  c16[ c16]  c8[ c8] }
}}

