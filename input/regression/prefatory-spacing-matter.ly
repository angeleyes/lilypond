\version "1.7.6"
\header {
texidoc = "prefatory spacing

TODO: show all common combinations to check for spacing anomalies.
"
}

\score { \notes \relative c'' {
	\property Staff.instrument = "fobar"
	\bar "||:"
	\key cis \major
	cis4 cis4 cis4 cis4 \clef bass  cis4 cis4 cis4 
}
\paper  { linewidth = -1. }
}
%% new-chords-done %%
