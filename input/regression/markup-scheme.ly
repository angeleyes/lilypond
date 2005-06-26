
\header {

    texidoc = "There is a Scheme macro @code{markup} to produce markup
    texts using a similar syntax as @code{\\markup}.

"

    }

%{

For maintenance reasons, we don't excercise the entire markup command set.

%}

\version "2.6.0"
\score {
     {
        \fatText
        f'1-\markup {
            foo
            \raise #0.2 \hbracket \bold bar
            \override #'(baseline-skip . 4)

            \bracket \column { baz bazr bla }
            \hspace #2.0
            \override #'(font-encoding . fetaMusic) {
                \lookup #"noteheads-0"
            }
            \musicglyph #"accidentals.-1"
            \combine "X" "+"   
            \combine "o" "/"
            \box \column { \line { "string 1" } \line { "string 2" } }
            \italic Norsk
            \super "2"
            \dynamic sfzp
            \huge { "A" \smaller "A" \smaller \smaller "A"
                    \smaller \smaller \smaller "A" }
            \sub "alike"
        }	
        \break
        f'1-#(markup* 
              "foo"
              #:raise 0.2 #:hbracket #:bold "bar"
              #:override '(baseline-skip . 4) 
              #:bracket #:column ( "baz" "bazr" "bla" )
              #:hspace 2.0
              #:override '(font-encoding . fetaMusic) #:line (#:lookup "noteheads-0" 
                                                       )
              #:musicglyph "accidentals.-1"
              #:combine "X" "+"   
              #:combine "o" "/"
              #:box #:column ("string 1" "string 2")
	      " "
              #:italic "Norsk"
              #:super "2"
	      #:circle #:dynamic "p"
	      " "
              #:dynamic "sfzp"
              #:huge #:line ("A" #:smaller "A" #:smaller #:smaller "A" 
                             #:smaller #:smaller #:smaller "A")
              #:sub "alike")
    }
    \layout { 
        raggedright = ##t
        indent = #0
        \context {
            \Staff
            \remove Time_signature_engraver 
        }
    }
}
