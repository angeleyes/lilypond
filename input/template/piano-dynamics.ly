#(ly:set-option 'old-relative)
\version "1.9.2"
\header {
texidoc ="
  Template that puts dynamics on a separate line, so it is neatly
centered between staffs.
"
}

upper = \notes\relative c'' {
  a b c d
}

lower = \notes\relative c {
  a2 c
}

dynamics = \notes {
  s2\fff\> s4
  s\!\pp
}

pedal = \notes {
 s2\sustainDown s2\sustainUp
}

\score {
  \context PianoStaff <
    \context Staff=upper \upper
    \context Dynamics=dynamics \dynamics
    \context Staff=lower <
      \clef bass
      \lower
    >
    \context Dynamics=pedal \pedal
  >
  \paper {
    \translator {
      \type "Engraver_group_engraver"
      \name Dynamics
      \consists "Output_property_engraver"
      
      minimumVerticalExtent = #'(-1 . 1)
      pedalSustainStrings = #'("Ped." "*Ped." "*")
      pedalUnaCordaStrings = #'("una corda" "" "tre corde")
      
      \consists "Piano_pedal_engraver"
      \consists "Script_engraver"
      \consists "Dynamic_engraver"
      \consists "Text_engraver"

      TextScript \override #'font-relative-size = #1
      TextScript \override #'font-shape = #'italic
      DynamicText \override #'extra-offset = #'(0 . 2.5)
      Hairpin \override #'extra-offset = #'(0 . 2.5)

      \consists "Skip_event_swallow_translator"

      \consistsend "Axis_group_engraver"
    }
    \translator {
      \PianoStaffContext
      \accepts Dynamics
      VerticalAlignment \override #'forced-distance = #7
    }
  }
  \midi {
    \translator {
      \type "Performer_group_performer"
      \name Dynamics
      \consists "Piano_pedal_performer"
      \consists "Span_dynamic_performer"
      \consists "Dynamic_performer"
    }
    \translator {
      \PianoStaffContext
      \accepts Dynamics
    }
  }
}
