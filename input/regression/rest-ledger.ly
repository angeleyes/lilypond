\version "2.1.22"
\header {
texidoc = "Whole and half rests moving outside the staff should get
ledger lines."
}


\score {
\notes  {
   \override Rest  #'staff-position  = #4
  r1 \override Rest  #'staff-position  = #5
  r1 \override Rest  #'staff-position  = #6
  
  r1 \override Rest  #'staff-position  = #-6
  r1 \override Rest  #'staff-position  = #-7
  r1 \override Rest  #'staff-position  = #-8
  r1

   \override Rest  #'staff-position  = #6
  r2 \override Rest  #'staff-position  = #7
  r2 \override Rest  #'staff-position  = #8
  
  r2 \override Rest  #'staff-position  = #-4
  r2 \override Rest  #'staff-position  = #-5
  r2 \override Rest  #'staff-position  = #-6
  r2

  

  
}}

