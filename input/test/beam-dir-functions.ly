
\version "2.1.7"
\header {
  texidoc = "@cindex Beam Dir Functions

There are several ways to calculate the direction of a beam:
@table @code
@item majority
number count of up or down notes
@item mean
mean center distance of all notes
@item median
mean centre distance weighted per note
@end table

You can spot the differences of these settings from these simple
examples:

These beam direction functions are defined in @file{scm/beam.scm}.  If
your favourite algorithm isn't one of these, you can hook up your own.
"
}

\paper { raggedright = ##t}
\score {
  \notes\relative c'' {\time 3/4
    \property Voice.Beam \set #'dir-function = #beam-dir-majority
      c8[ g]
    \property Voice.Beam \set #'dir-function = #beam-dir-mean
      c[ g] 
    \property Voice.Beam \set #'dir-function = #beam-dir-median
      c[ g]
    
    \time 3/8
    \property Voice.Beam \set #'dir-function = #beam-dir-majority
      c8[ c g]
    \property Voice.Beam \set #'dir-function = #beam-dir-mean
      c[ c g] 
    \property Voice.Beam \set #'dir-function = #beam-dir-median
      c[ c g] 
  }
\paper{raggedright = ##t}
}

%% Local variables:
%% LilyPond-indent-level:2
%% End:



