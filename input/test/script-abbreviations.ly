
% this chart is used in the manual too.

\version "2.4.0"
\header { texidoc = "@cindex Script Abbreviations

Some articulations may be entered using an abbreviation.

"
	  
}

\score {
     \context Voice {
      \override TextScript  #'font-family = #'typewriter
      \override TextScript  #'font-shape = #'upright
      c''4-._"c-."      s4
      c''4--_"c-{}-"    s4
      c''4-+_"c-+"      s4
      c''4-|_"c-|"      s4
      c''4->_"c->"      s4
      c''4-^_"c\\^{ }" s4
      c''4-__"c\_" s4      
    }
  }

