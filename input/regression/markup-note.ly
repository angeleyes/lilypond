\header {

    texidoc = "The note markup function may be used to make metronome
 markings. It works for a variety of flag, dot and duration settings."
}
\version "2.1.22"

\score { \notes { c4^\markup {
    \note #"1" #1
    \note #"2" #1
    \note #"4" #1
    \note #"8" #1
    \note #"16" #1
    \note #"32" #1
    \note #"64" #1

    \note #"1" #-1
    \note #"2" #-1
    \note #"4" #-1
    \note #"8" #-1
    \note #"16" #-1
    \note #"32" #-1
    \note #"64" #-1

    \note #"1." #-1
    \note #"2." #-1
    \note #"4." #-1
    \note #"8." #-1
    \note #"16." #-1
    \note #"32." #-1
    \note #"64." #-1

    \note #"1." #1
    \note #"2." #1
    \note #"4." #1
    \note #"8." #1
    \note #"16." #1
    \note #"32." #1
    \note #"64." #1

}

} }
