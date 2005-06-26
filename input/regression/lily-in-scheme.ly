\header { texidoc = "LilyPond syntax can be used inside scheme to
    build music expressions, with the @code{#@{ ... #@}} syntax.
    Scheme forms can be introduced inside these blocks by escaping
    them with a @code{$}, both in a LilyPond context or in a Scheme
    context.

In this example, the @code{\withpaddingA}, @code{\withpaddingB} and
@code{\withpaddingC} music functions set different kinds of padding on
the @code{TextScript} grob.

"
          
      }

\version "2.6.0"
\layout { raggedright = ##t }

withPaddingA = #(def-music-function (parser location padding music) (number? ly:music?)
		 #{ \override TextScript #'padding = #$padding
		 $music 
		 \revert TextScript #'padding #})

withPaddingB = #(def-music-function (parser location padding music) (number? ly:music?)
		 #{ \override TextScript #'padding = #$(* padding 2)
		 $music 
		 \revert TextScript #'padding #})

withPaddingC = #(def-music-function (parser location padding music) (number? ly:music?)
		 #{ \override TextScript #'padding = #(+ 1 $(* padding 2))
		    $music 
		    \revert TextScript #'padding #})

{
    c'^"1"
    \withPaddingA #2
    { c'^"2" c'^"3"}
    c'^"4"
    \withPaddingB #2
    { c'^"5" c'^"6"}
    c'^"7"
    \withPaddingC #2
    { c'^"8" c'^"9"}
    c'^"10"
}
