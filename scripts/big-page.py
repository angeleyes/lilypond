import re
import sys
import os

dir = sys.argv[1]
os.chdir (dir)

s = ''
for a in ['index.html',
	  'introduction.html', 'software.html', 'problem-statement.html',
	  'implementing-notation.html', 'engraving.html',
	  'implementing-typography.html', 'formatting-architecture.html',
	  'benchmarking.html', 'typography-features.html', 'input-format.html',
	  'conclusion.html']:

	s += open (a).read()

s= re.sub ('<a href="?([^.">]+).html"?>',
	   '<a href="#\\1">', s)
 
open ('big-page.html','w').write (s)

	
