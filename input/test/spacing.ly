\version "1.7.18"
\header{
texidoc =  "Concurrent tuplets should be spaced equidistantly on
all staffs" 
}



multipart =  \notes \relative c'{ 
    \context StaffGroup < 
                \context Staff = one \context Voice { 
                   c4
                   c 
                   c c 
                }
                \context Staff = two \context Voice { 
                    c8-[ c] 
                    c-[ c] 
                    c-[ c]  c-[ c] 
                }
                \context Staff = three \context Voice { 
                   \times 2/3 {  c8-[ c c] } 
                   \times 2/3 {  c-[  c c] } 
                   \times 2/3 {  c-[  c c] }
		   \times 2/3 {  c-[ c c] } 
                }
                \context Staff = four \context Voice { 
                   \times 2/4 {  c8-[ c c c] }
                   \times 2/4 {  c-[  c c c] } 
                   \times 2/4 {  c-[  c c c] } \times 2/4 {  c-[ c c c] } 
                }
                \context Staff = five \context Voice { 
                   \times 2/5 {  c8-[ c c c c] } 
                   \times 2/5 {  c-[  c c c c] } 
                   \times 2/5 {  c-[  c c c c] } \times 2/5 {  c-[ c c c c] } 
		}
                \context Staff = six \context Voice { 
                   \times 2/6 {  c8-[ c c c c c] } 
                   \times 2/6 {  c-[  c c c c c] } 
                   \times 2/6 {  c-[  c c c c c] } \times 2/6 {  c-[ c c c c c] } 
                }
                \context Staff = seven \context Voice { 
                   \times 2/7 {  c8-[ c c c c c c] } 
                   \times 2/7 {  c-[  c c c c c c] } 
                   \times 2/7 {  c-[  c c c c c c] } 
		   \times 2/7 {  c-[  c c c c c c] } 
                }
                \context Staff = eight \context Voice { 
                   \times 2/8 {  c8-[ c c c c c c c] } 
                   \times 2/8 {  c-[  c c c c c c c] } 
                   \times 2/8 {  c-[  c c c c c c c] } 
		   \times 2/8 {  c-[  c c c c c c c] } 
		}
                \context Staff = nine  \context Voice { 
                   \times 2/9 {  c8-[ c c c c c c c c] } 
                   \times 2/9 {  c-[  c c c c c c c c] } 
                   \times 2/9 {  c-[  c c c c c c c c] } 
		   \times 2/9 {  c-[  c c c c c c c c] } 
		}
                \context Staff = ten  \context Voice { 
                   \times 2/10 {  c8-[ c c c c c c c c c] } 
                   \times 2/10 {  c-[  c c c c c c c c c] } 
                   \times 2/10 {  c-[  c c c c c c c c c] } 
		   \times 2/10 {  c-[  c c c c c c c c c] } 
		}
                \context Staff = eleven  \context Voice { 
                   \times 2/11 {  c8-[ c c c c c c c c c c] } 
                   \times 2/11 {  c-[  c c c c c c c c c c] } 
                   \times 2/11 {  c-[  c c c c c c c c c c] } 
		   \times 2/11 {  c-[  c c c c c c c c c c] } 
                }
            >
	}

singlepart =  \notes{ 
	c4 d e f
	c4 d e  f8-[ f]
	c4 d e f8  f16-[ f]
	c4 d e f8 f16  f32-[ f]
	c4 d e f8 f16 f32  f64-[ f]
	c4 c4 c4 c4
	
    }
    
    \score{
    \notes { 
%    	\singlepart 
	\multipart 
    }
    \paper {
      indent = 0.0 \cm
      linewidth= 20.0\cm
    }
}
%% new-chords-done %%
