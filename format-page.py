import getopt
import re
import sys
import string
import os

header = '''<HTML>
<HEAD>
<TITLE>NO TITLE</TITLE>
<LINK REL="stylesheet" TYPE="text/css" HREF="/newweb/out/site/newweb.css">
</HEAD>
'''

main_template = '''<DIV class="main">
<HR>
%s
<HR>
</DIV>
'''

footer = '''
</BODY>'''


menu_template = '''<DIV class="menu">
%s
</DIV>
'''

location_template = '''<DIV class="location">
%s
</DIV>
'''

button_template = '''<TD><A href="%(url)s"><IMG ALT="%(text)s" SRC="/newweb/out/images/%(text)s.png" ONMOUSEOVER="this.src='/newweb/out/images/%(text)s-hover.png'" ONMOUSEOUT="this.src='/newweb/out/images/%(text)s.png'"></A></TD>'''
button_active_template = '''<TD><A href="%(url)s"><IMG ALT="[%(text)s]" SRC="/newweb/out/images/%(text)s-hover.png" ONMOUSEOVER="this.src='/newweb/out/images/%(text)s.png'" ONMOUSEOUT="this.src='/newweb/out/images/%(text)s-hover.png'"></A></TD>'''

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
		label = re.sub ("['! ]", "_", label)
		active = 1
		if file == here:
			active = active and (depth > 0)
 			button = button_active_template % {
				'url' : '../' * depth + file,
				'text' : label }
		else:
 			button = button_template % {
				'url' : '../' * depth + file,
				'text' : label }

		buttons[label] = 1
		return button
	
	labels = map (entry_to_label, menu)
	here_label = filter (lambda x: x[0] == here, menu)
	if not here_label:
		here_label = ''
	else:
		here_label = here_label[0][1]
		
	# FIXME
	tr_str = '<TR>%s</TR>' % string.join (labels, '')
 	menu_str = '<TABLE>%s</TABLE>' % tr_str

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
		
	html = open (in_file_name).read ()
	
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
	
	menu = menu_template % string.join (tabs)
	nav_str = location_template % nav_str
	main = main_template % html
	page = header + menu + main + nav_str + footer
	
	open (os.path.join (outdir, in_file_name), 'w').write (page)

for (o,a) in options:
	if o == '--outdir':
		outdir = a
	else:
		assert unimplemented

buttons = {}

for f in files:
	sys.stderr.write ('%s...\n' % f) 
	do_one_file (f)

bfn = outdir + '/buttons'
sys.stderr.write ('writing: %s...\n' % bfn)
bf = open (bfn, "w")
for i in buttons.keys ():
	bf.write (i + '\n')
bf.close ()
