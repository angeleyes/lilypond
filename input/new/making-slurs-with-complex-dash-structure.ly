\version "2.13.1"

\header{
  texidoc = "
Slurs can be made with complex dash patterns by defining
the @code{dash-definition} property.  @code{dash-definition}
is a list of @code{dash-elements}.  A @code{dash-element} is a
list of parameters defining the dash behavior for a segment of
the slur.

The slur is defined in terms of the bezier parameter t
which ranges from 0
at the left end of the slur to 1 at the right end of the slur.
@code{dash-element} is a list @code{(start-t stop-t dash-fraction
dash-period)}.  The region of the slur from @code{start-t} to
@code{stop-t} will have a fraction @code{dash-fraction}
of each @code{dash-period} black.  @code{dash-period} is
defined in terms of staff spaces.  @code{dash-fraction} is
set to 1 for a solid slur.
"
  doctitle = "Making slurs with complex dash structure"
}

\relative c' {
  \once \override 
    Slur #'dash-definition = #'((0 0.3 0.1 0.75)
                                (0.3 0.6 1 1)
                                (0.65 1.0 0.4 0.75))
  c( d e f)
  \once \override 
    Slur #'dash-definition = #'((0 0.25 1 1)
                                (0.3 0.7 0.4 0.75)
                                (0.75 1.0 1 1))
  c( d e f)
}


