#!@PYTHON@
# mutopia-index.py

name = 'mutopia-index'

import fnmatch
import os

_debug = 0

_prune = ['(*)']

def find(pattern, dir = os.curdir):
        list = []
        names = os.listdir(dir)
        names.sort()
        for name in names:
                if name in (os.curdir, os.pardir):
                        continue
                fullname = os.path.join(dir, name)
                if fnmatch.fnmatch(name, pattern):
                        list.append(fullname)
                if os.path.isdir(fullname) and not os.path.islink(fullname):
                        for p in _prune:
                                if fnmatch.fnmatch(name, p):
                                        if _debug: print "skip", `fullname`
                                        break
                        else:
                                if _debug: print "descend into", `fullname`
                                list = list + find(pattern, fullname)
        return list


import re
import os
import sys
import stat

def gulp_file (fn):
	try:
		f = open (fn)
	except:
		raise 'not there' , fn
	return f.read ()

def file_exist_b (fn):
	try:
		f = open (fn)
		return 1
	except:
		return 0


headertext= r"""

<p>You're looking at a page with some LilyPond samples.  These files
are also included in the distribution. The output is completely
generated from the <tt>.ly</tt> source file, without any further touch
up.

<p>The PostScript files were generated using TeX and dvips at 600 dpi.
The pictures are 90 dpi anti-aliased snapshots of the printed output.
The images are in PNG format, and should be viewable with any current
browser.

<p>If you want a better impression of the appearance, do print out one of
the postscript files.
"""

headertext_nopics= r"""
<p>Nothing to be seen here, move along.
"""

#
# FIXME breaks on multiple strings.
#
def read_lilypond_header (fn):
	s = open(fn).read ()
	s = re.sub('%.*$', '', s)
	s = re.sub('\n', ' ', s)		

	dict = {}
	m = re.search (r"""\\header\s*{([^}]*)}""", s)

	if m:
			s = m.group(1)
	else:
			return dict

	while s:
		m = re.search (r"""\s*(\S+)\s*=\s*([^;]+)\s*;""", s)
		if m == None:
			s = ''
		else:
			s = s[m.end (0):]
			left  = m.group	 (1)
			right = m.group (2)

			left = re.sub ('"', '', left)
			right = re.sub ('"', '', right)
			dict[left] = right

	return dict

def help ():
	sys.stdout.write (r"""Usage: mutopia-index [OPTIONS] INFILE OUTFILE
Generate index for mutopia.

Options:
  -h, --help                 print this help
  -o, --output=FILE          write output to file
  -s, --subdirs=DIR	     add subdir
      --suffix=SUF	     specify suffix
      
"""
					  )
	sys.exit (0)

# ugh.
def gen_list(inputs, filename):
	print "generating HTML list %s\n" % filename
	if filename:
		list = open(filename, 'w')
	else:
		list = sys.stdout
	list.write ('<html><title>Rendered Examples</title>\n')
	list.write ('<body bgcolor=white>\n')
	
	if inputs:
			list.write (headertext)
	else:
			list.write (headertext_nopics)


	for ex in inputs:
		(base, ext) = os.path.splitext (ex)
		(base, ext2) = os.path.splitext (base)		
		ext = ext2 + ext
		
		print '%s, ' % ex
		header = read_lilypond_header(ex)
		
		def read_dict(s, default, h =header):
				try:
					ret = h[s]
				except KeyError:
					ret = default
				return ret
		head = read_dict('title', os.path.basename (base))
		composer = read_dict('composer', '')
		desc = read_dict('description', '')
		list.write('<hr>\n')
		list.write('<h1>%s</h1>\n' % head);
		if composer:
			list.write('<h2>%s</h2>\n' % composer)
		if desc:
			list.write('%s<p>' % desc)
		list.write ('<ul>\n')
		def list_item(filename, desc, type, l = list):
			if file_exist_b(filename):
				
				l.write ('<li><a href="%s">%s</a>' % (filename, desc))

				# todo: include warning if it uses \include
				# files.
				
				size=os.stat(filename)[stat.ST_SIZE]
				kB=(size + 512) / 1024
				if kB:
					l.write (' (%s %d kB)' % (type, kB))
				else:
					l.write (' (%s %d characters)' % (type, size))
				pictures = ['jpeg', 'png', 'xpm']
				l.write ('\n')

		list_item(base + ext, 'The input', 'ASCII')
		for pageno in range(1,100):
			f  = base + '-page%d.png' % pageno
			if not file_exist_b (f):
				break
			list_item(f, 'See a picture of page %d' % pageno, 'png')
		list_item(base + '.pdf', 'Print', 'PDF')
		list_item(base + '.ps.gz', 'Print', 'gzipped PostScript')
		list_item(base + '.midi', 'Listen', 'MIDI')
		list.write ("</ul>\n");

	list.write('</body></html>\n');
	list.close()

import getopt

(options, files) = getopt.getopt(sys.argv[1:], 
  'ho:', ['help', 'output='])
outfile = 'examples.html'

subdirs =[]
for opt in options:
	o = opt[0]
	a = opt[1]
	if o == '--help' or o == '-h':
		help()
	elif o == '--output' or o == '-o':
		outfile = a

dirs  = []
for f in files:
	dirs = dirs + find ('out-www', f)

if not dirs:
	dirs = ['.']

allfiles = []

for d in dirs:
	allfiles = allfiles + find ('*.ly.txt', d)

print allfiles

gen_list (allfiles, outfile)

