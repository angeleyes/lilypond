#!/usr/bin/python
import sys
import os
import os.path
import shutil

# 'expressive' not available yet
dirs = ['ancient','chords','connecting','contemporary','expressive','guitar','parts','repeats','spacing','staff','text','vocal']
notsafe=[]

try:
	in_dir = sys.argv[1]
except:
	print "Please specify input_file."
	sys.exit()

for dir in dirs:
	srcdir = os.path.join (in_dir, dir)
	destdir = os.path.join ('input', 'lsr', dir)
	if not(os.path.isdir(destdir)):
		print "Please run this script from the head of the source tree,"
		print "  and/or check that you have the right categories."
		sys.exit()

	file_names = os.listdir (destdir)
	for file in file_names:
		if (file.endswith ('.ly')):
			if (file[:3] != 'AAA'):
				os.remove( os.path.join(destdir,file) )

	file_names = os.listdir (in_dir + dir)
	for file in file_names:
		src = os.path.join (srcdir, file)
		dest = os.path.join (destdir, file)
		shutil.copyfile (src, dest)
		s = os.system('lilypond -dsafe --ps -o /tmp/lsrtest ' + dest)
		if s:
			notsafe.append(dest)
			#raise 'Failed'

file=open("lsr-unsafe.txt", 'w')
for s in notsafe:
	file.write(s+'\n')
file.close()
print
print
print "Unsafe files printed in lsr-unsafe.txt: CHECK MANUALLY!"
print "  (probably with  xargs git-diff < lsr-unsafe.txt  )"
print

