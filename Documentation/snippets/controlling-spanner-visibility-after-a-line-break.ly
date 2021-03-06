% Do not edit this file; it is automatically
% generated from Documentation/snippets/new
% This file is in the public domain.
%% Note: this file works from version 2.13.1
\version "2.13.10"

\header {
  lsrtags = "expressive-marks, tweaks-and-overrides"
  texidoc = "The visibility of spanners which end on the first note
following a line break is controlled by the @code{after-line-breaking}
callback @code{ly:spanner::kill-zero-spanned-time}.

For objects such as glissandos and hairpins, the default behaviour is
to hide the spanner after a break; disabling the callback will allow
the left-broken span to be shown.

Conversely, spanners which are usually visible, such as text spans,
can be hidden by enabling the callback.
"

  doctitle = "Controlling spanner visibility after a line break"
} % begin verbatim


\paper { ragged-right = ##t }

\relative c'' {
  \override Hairpin #'to-barline = ##f
  \override Glissando #'breakable = ##t
  % show hairpin
  \override Hairpin #'after-line-breaking = ##t
  % hide text span
  \override TextSpanner #'after-line-breaking =
    #ly:spanner::kill-zero-spanned-time
  e2\<\startTextSpan
  % show glissando
  \override Glissando #'after-line-breaking = ##t
  f2\glissando
  \break
  f,1\!\stopTextSpan
}

