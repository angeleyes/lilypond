%%  Do not edit this file; it is auto-generated from LSR!
\version "2.11.23"

\header { texidoc = "
This snippet shows LilyPond's \"limited support\" (says the
Documentation) for figured bass... well, maybe not so limited after
all; moreover, though there are numerous BassFigure objects you can
always use text markups instead to get precisely what you need.

You can find all of these examples in the manual; read the \"Figured
bass\" section to learn how to add a figured bass with simple but
powerful notation.
" }

{
     <<
       \new Voice { \clef bass dis4 c d ais g fis e a r gis d b, f,! e c c c1}
       \figures {
         < 6 >4 < 7\+ >8 < 6+ [_!] >
         < 6 >4 <6 5 [3+] >
         < _ >4 < 6 5/>4
         <4- 6+ 7!> <5++> <3--> <7/> r <6\+ 5/> 
         < [4 6] 8 [_! 12] > < 5 \markup { \number 6 \super (1) } >
             \set useBassFigureExtenders = ##t
    <4 6> <3 6> <3 7>
       }
     >>
}