import getopt
import re
import sys
import string
import os

outdir = '/tmp'
(options, files) = getopt.getopt (sys.argv[1:], '', ['outdir=']) 

def read_menu (f):
	menu = eval (open (f).read ())
	nm = []
	for (f, l) in menu:
		if f[-1] == '/':
			f = f[:-1]
		nm.append ((f,l))

	return nm
		
	
def one_tab (depth, file):
	(path, here) = os.path.split (file)

	menu_file = os.path.join (path, 'menu-entries.py')
	if not os.path.exists (menu_file):
		return ('', '?')
	
	menu = read_menu (menu_file)

	here_label = ''
	def entry_to_label (x):
		(file, label) = x
		txt = label


		active = 1
		if file == here:
			active = active and (depth > 0)
			txt = '[<b>%s</b>]' % txt
		
		txt = '<td align="center"><a href="%s">%s</a></td>' % ('../' * depth + file, txt)

		return txt
	
	labels = map (entry_to_label, menu)
	here_label = filter (lambda x: x[0] == here, menu)
	if not here_label:
		here_label = ''
	else:
		here_label = here_label[0][1]
		
	
	menu_str = string.join (labels, ' ')
	menu_str =  ('<tr>%s</tr>' % menu_str) 
	menu_str = '<table border=0 cellspacing="15%%">%s</table>' % menu_str

	return (menu_str, here_label)

def do_one_file (in_file_name):
	(path, here) = os.path.split (in_file_name)

	here_dir  = path
	if os.path.isdir (in_file_name):
		here_dir = in_file_name

	try:
		os.makedirs (os.path.join (outdir, path))
	except:
		OSError
		
	contents = open (in_file_name).read ()
	
	file_name = in_file_name
	tabs = []

	depth = 0
	is_index = re.search ('index.html$', file_name)

	locations = []
	while 1:
		(path, here) = os.path.split (file_name)
		if os.path.exists (os.path.join (path, 'GNUmakefile')):
			break

		(menu, here_label) = one_tab  (depth, file_name)
		if menu:
			tabs.append (menu)

		if here_label:
			locations.append (here_label)
		file_name = path

		depth += 1

	tabs.reverse ()
	locations.reverse()

	locations = ['Home'] + locations
	nav_elts = []
	d = len (locations) - 1
	for l in locations:
		if d == 0:
			l = '[<b>%s</b>]' % l

		loc = ''
		if is_index:
			loc = '../' * d
		elif d == 1:
			loc = './'
		elif d > 1:
			loc = '../' * (d-1)
		nav_elts.append ("<a href=%s>%s</a>" % (loc, l))
		d -= 1
	
	nav_str = string.join (nav_elts, ' &gt; ')		
	
	menu = '<center>%s</center>' % string.join(tabs)
	nav_str = '<center>%s</center>' % nav_str
	contents  =  menu  + contents + '<p><br><p>' + nav_str
	
	open (os.path.join (outdir, in_file_name), 'w').write (contents)

for (o,a) in options:
	if o == '--outdir':
		outdir = a
	else:
		assert unimplemented
		
for f in files:
	sys.stderr.write ('%s...\n' % f) 
	do_one_file (f)


