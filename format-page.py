import getopt
import re
import sys
import string
import os

doco = """

Notes for this script.


* Do not use absolute URL locations. They will break local installs of
  the website. Rather, use '../' * depth to get to the root.

"""


header = '''<HTML>
<HEAD>
<TITLE>%(title)s</TITLE>
<LINK REL="stylesheet" TYPE="text/css" HREF="%(depth)snewweb.css">
</HEAD>
<BODY> 
'''

main_template = '''<DIV class="main">
<HR>
%s
<br clear=all>
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

# don't use mouseover magic as long as we don't have button images

## don't break line after > 
button_template = '''<TD class="%(class)s"><A
href="%(url)s"
>%(text)s</A>%(suffix)s</TD>'''

#
button_active_template = '''<TD class="%(class)s"><A href="%(url)s"
>
%(text)s</A>%(suffix)s</TD>'''


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
	"""
	Return the menu to put at the top, and
	the label that you for the > > > bars at the bottom.


	TODO:
	use a dict in menu-entries.py for pages without menu at the top
	(eg. page 1, .. , page 12)
	"""
	
	(path, here) = os.path.split (file)


	menu_file = os.path.join (path, 'menu-entries.py')
	if not os.path.exists (menu_file):
		if re.search ('index.html$', file):
			return ('', '')
		else:
			base = os.path.splitext (here)[0]
			return ('', base)
	
	menu = read_menu (menu_file)

	here_label = ''
	def entry_to_label (x):
		(file, label) = x
		name = re.sub ("['! ]", "-", label)
		active = 1

		button_dict =  {
			'url' : '../' * depth + file,
			'name' : name,
			'text' : label,
			'suffix': '',
			'root' : '../' * depth
		}
		
		if file == here:
			active = active and (depth > 0)

			button_dict['class'] =  "menuactive"
			button_dict['text'] = '[<b>%s</b>]' % button_dict['text']
		else:
 			button_dict['class'] =  "menu"

		# ugh.
		if label == 'Home':
			button_dict['suffix'] = '</td><td class=menu><b>&gt;</b>'
		
		button = button_template % button_dict

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

	# UGH.
	if locations <> ['Home']:
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
	nav_title = 'LilyPond - ' + string.join (locations[1:], ' - ')
	
	menu = menu_template % string.join (tabs)
	nav_str = location_template % nav_str
	main = main_template % html
	depth_str = ('../' * (depth-1))

	## AT substitutions.

	titles = [nav_title]
	
	def grab_title (match):
		titles.append (match.group (1))
		return ''

	main = re.sub ('<title>(.*?)</title>', grab_title, main)
	main = re.sub ('@DEPTH@', depth_str, main)
	main = re.sub ('@IMAGES@', os.path.join (depth_str, 'images/'),
		       main)
	
	page = header % {'depth': depth_str,
			 'title': titles[-1], 

			 } +menu + main + nav_str + footer
	
	open (os.path.join (outdir, in_file_name), 'w').write (page)

for (o,a) in options:
	if o == '--outdir':
		outdir = a
	else:
		assert unimplemented


for f in files:
	sys.stderr.write ('%s...\n' % f) 
	do_one_file (f)

