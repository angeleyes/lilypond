\version "2.12.0"
\header{
  texidoc="
Breathing signs are available in different tastes: commas (default),
ticks, vees and `railroad tracks' (caesura)."
}


{
  %% Modern notation:
  \new Staff {
    \relative c'' {
      %% by default, \breathe uses the rcomma, just as if saying:
      %% \override BreathingSign #'text =
      %%   #(make-musicglyph-markup "scripts.rcomma")
      %%
      c4 c \breathe c c |

      %% rvarcomma and lvarcomma are variations of the default rcomma
      %% and lcomma
      %%
      \override Staff.BreathingSign #'text =
        #(make-musicglyph-markup "scripts.rvarcomma")
      c4 c \breathe c c |

      %% wedge
      %%
      \override BreathingSign #'text =
        #(make-musicglyph-markup "scripts.upbow")
      c4 c \breathe c c |

      %% caesurae
      %%
      \override BreathingSign #'text =
        #(make-musicglyph-markup "scripts.caesura.curved")
      c4 c \breathe c c |

      \override BreathingSign #'text =
        #(make-musicglyph-markup "scripts.caesura.straight")
      c4 c \breathe c c |
    }
  }
}
