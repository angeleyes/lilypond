\version "1.7.6"


\score {

  \notes     \context Staff = treble    {
      \property Staff.instrument = "instr " { c''4 }}

\paper {
linewidth=-1.0
\translator { \StaffContext \consists "Instrument_name_engraver" }
}}

%% new-chords-done %%
