# -*-python-*-

'''
Experimental scons (www.scons.org) building.

Usage

    scons TARGET

build from source directory ./TARGET (not recursive)

Configure, build

    scons [config]             # configure
    scons                      # build all

Run from build tree

    run=$(pwd)/out-scons/usr
    export LOCALE=$run/share/locale
    export TEXMF='{'$run/share/lilypond,$(kpsexpand '$TEXMF')'}'
    PATH=$run/bin:$PATH

    #optionally, if you do not use custom.py below
    #export LILYPONDPREFIX=$run/share/lilypond

    lilypond-bin input/simple

Other targets
    scons mf-essential         # build minimal mf stuff

    scons doc                  # build web doc
    scons config               # reconfigure
    scons install              # install
    scons -c                   # clean
    scons -h                   # help

    scons /                    # build *everything* (including installation)

Options  (see scons -h)
    scons build=DIR            # clean srcdir build, output below DIR
    scons out=DIR              # write output for alterative config to DIR

Debugging
    scons --debug=dtree
    scons --debug=explain
    scons verbose=1

Optional custom.py

import os
out='out-scons'
optimising=0
debugging=1
gui=1
os.path.join (os.getcwd (), '=install')
prefix=os.path.join (os.environ['HOME'], 'usr', 'pkg', 'lilypond')

'''


# TODO:
#  * usability

#  * more program configure tests (mfont, ...?)

#  * install doc

#  * split doc target: doc input examples mutopia?

#  * grep FIXME $(find . -name 'S*t')


import re
import glob
import os
import string
import sys
import stat
import shutil

# duh, we need 0.95.1
EnsureSConsVersion (0, 95)

usage = r'''Usage:
scons [KEY=VALUE].. [TARGET|DIR]..

TARGETS: clean, config, doc, dist, install, mf-essential, po-update,
         realclean, release, tar, TAGS

'''
      

config_cache = 'scons.cache'

config_vars = [
	'BASH',
	'CCFLAGS',
	'CPPPATH',
	'CPPDEFINES',
	'CXXFLAGS',
	'DEFINES',
	'LIBS',
	'LINKFLAGS',
	'METAFONT',
	'PERL',
	'PYTHON',
	]

# Put your favourite stuff in custom.py
opts = Options ([config_cache, 'custom.py'], ARGUMENTS)
opts.Add ('prefix', 'Install prefix', '/usr/')
opts.Add ('out', 'Output directory', 'out-scons')
opts.Add ('build', 'Build directory', '.')
opts.Add ('DESTDIR', 'DESTDIR prepended to prefix', '')
opts.AddOptions (
	BoolOption ('warnings', 'compile with -Wall and similiar',
		   1),
	BoolOption ('debugging', 'compile with debugging symbols',
		    0),
	BoolOption ('optimising', 'compile with optimising',
		    1),
	BoolOption ('shared', 'build shared libraries',
		    0),
	BoolOption ('static', 'build static libraries',
		    1),
	BoolOption ('gui', 'build with GNOME backend (EXPERIMENTAL)',
		    1),
	BoolOption ('verbose', 'run commands with verbose flag',
		    0),
	BoolOption ('checksums', 'use checksums instead of timestamps',
		    1),
	)

srcdir = Dir ('.').srcnode ().abspath
#ugh
sys.path.append (os.path.join (srcdir, 'stepmake', 'bin'))
import packagepython
package = packagepython.Package (srcdir)
version = packagepython.version_tuple_to_str (package.version)

ENV = { 'PATH' : os.environ['PATH'] }
for key in ['LD_LIBRARY_PATH', 'GUILE_LOAD_PATH', 'PKG_CONFIG_PATH']:
    	if os.environ.has_key (key):
        	ENV[key] = os.environ[key]

env = Environment (
	ENV = ENV,

	BASH = '/bin/bash',
	PERL = '/usr/bin/perl',
	PYTHON = '/usr/bin/python',
	SH = '/bin/sh',

	MAKEINFO = 'LANG= makeinfo',
	MF_TO_TABLE_PY = srcdir + '/buildscripts/mf-to-table.py',
	
	PKG_CONFIG_PATH = [os.path.join (os.environ['HOME'],
					 'usr/pkg/gnome/lib'),
			   os.path.join (os.environ['HOME'],
					 'usr/pkg/pango/lib')],
	GZIP='-9v',
	MFMODE = 'ljfour',
	TOPLEVEL_VERSION = version,
	)

# Add all config_vars to opts, so that they will be read and saved
# together with the other configure options.
map (lambda x: opts.AddOptions ((x,)), config_vars)

Help (usage + opts.GenerateHelpText (env))

opts.Update (env)

# Using content checksums prevents rebuilds after [re]configure if
# config.hh has not changed.
if env['checksums']:
	SetOption ('max_drift', 0)
	TargetSignatures ("content")

absbuild = Dir (env['build']).abspath
outdir = os.path.join (Dir (env['build']).abspath, env['out'])
run_prefix = os.path.join (absbuild, os.path.join (env['out'], 'usr'))


config_hh = os.path.join (outdir, 'config.hh')
version_hh = os.path.join (outdir, 'version.hh')

env.Alias ('config', config_cache)

cachedir = os.path.join (outdir, 'build-cache')

if not os.path.exists(cachedir):
	os.makedirs(cachedir)

CacheDir (os.path.join (outdir, 'build-cache'))

# No need to set $LILYPONDPREFIX to run lily, but cannot install...
if env['debugging'] and not 'install' in COMMAND_LINE_TARGETS:
	env['prefix'] = run_prefix

prefix = env['prefix']
bindir = os.path.join (prefix, 'bin')
sharedir = os.path.join (prefix, 'share')
libdir = os.path.join (prefix, 'lib')
localedir = os.path.join (sharedir, 'locale')
sharedir_doc_package = os.path.join (sharedir, 'doc', package.name)
sharedir_package = os.path.join (sharedir, package.name)
sharedir_package_version = os.path.join (sharedir_package, version)
lilypondprefix = sharedir_package_version

# junkme
env.Append (
	absbuild = absbuild,
	srcdir = srcdir,
	)


def list_sort (lst):
	sorted = lst
	sorted.sort ()
	return sorted


def configure (target, source, env):
	vre = re.compile ('^.*[^-.0-9]([0-9][0-9]*\.[0-9][.0-9]*).*$', re.DOTALL)
	def get_version (program):
		command = '(%(program)s --version || %(program)s -V) 2>&1' % vars ()
		pipe = os.popen (command)
		output = pipe.read ()
		if pipe.close ():
			return None
		v = re.sub (vre, '\\1', output)
		return string.split (v, '.')

	def test_program (lst, program, minimal, description, package):
		sys.stdout.write ('Checking %s version... ' % program)
		actual = get_version (program)
		if not actual:
			print 'not found'
			lst.append ((description, package, minimal, program,
				     'not installed'))
			return
		sys.stdout.write (string.join (actual, '.'))
		sys.stdout.write ('\n')
		if actual < string.split (minimal, '.'):
			lst.append ((description, package, minimal, program,
				     string.join (actual, '.')))

	for i in ['bash', 'perl', 'python', 'sh']:
		sys.stdout.write ('Checking for %s... ' % i)
		c = WhereIs (i)
		key = string.upper (i)
		if c:
			env[key] = c
			sys.stdout.write (c)
		else:
			sys.stdout.write ('not found: %s (using: %s)' \
					  % (c, env[key]))
			# Hmm? abort?
		sys.stdout.write ('\n')

	required = []
	test_program (required, 'gcc', '2.8', 'GNU C compiler', 'gcc')
	test_program (required, 'g++', '3.0.5', 'GNU C++ compiler', 'g++')
	test_program (required, 'python', '2.1', 'Python (www.python.org)', 'python')
	test_program (required, 'guile-config', '1.6', 'GUILE development',
			'libguile-dev or guile-devel')
	# Do not use bison 1.50 and 1.75.
	test_program (required, 'bison', '1.25', 'Bison -- parser generator',
			'bison')
	test_program (required, 'flex', '0.0', 'Flex -- lexer generator', 'flex')


	optional = []
	test_program (optional, 'makeinfo', '4.7', 'Makeinfo tool', 'texinfo')
	test_program (optional, 'guile', '1.6', 'GUILE scheme',
			'libguile-dev or guile-devel')
	test_program (optional, 'mftrace', '1.0.27', 'Metafont tracing Type1',
			'mftrace')
	test_program (optional, 'perl', '4.0',
			'Perl practical efficient readonly language', 'perl')
	#test_program (optional, 'foo', '2.0', 'Foomatic tester', 'bar')

	def CheckYYCurrentBuffer (context):
		context.Message ('Checking for yy_current_buffer... ')
		ret = conf.TryCompile ("""using namespace std;
		#include <FlexLexer.h>
		class yy_flex_lexer: public yyFlexLexer
		{
		public:
		yy_flex_lexer ()
		{
		yy_current_buffer = 0;
		}
		};""", '.cc')
		context.Result (ret)
		return ret

	conf = Configure (env, custom_tests = { 'CheckYYCurrentBuffer'
						: CheckYYCurrentBuffer })

	defines = {
	   'DIRSEP' : "'%s'" % os.sep,
	   'PATHSEP' : "'%s'" % os.pathsep,
	   'TOPLEVEL_VERSION' : '"' + version + '"',
	   'PACKAGE': '"' + package.name + '"',
	   'DATADIR' : '"' + sharedir + '"',
	   'LILYPOND_DATADIR' : '"' + sharedir_package + '"',
	   'LOCAL_LILYPOND_DATADIR' : '"' + sharedir_package_version + '"',
	   'LOCALEDIR' : '"' + localedir + '"',
	}
	conf.env.Append (DEFINES = defines)

	command = r"""python -c 'import sys; sys.stdout.write ("%s/include/python%s" % (sys.prefix, sys.version[:3]))'""" #"
	PYTHON_INCLUDE = os.popen (command).read ()
	env.Append (CPPPATH = PYTHON_INCLUDE)

	headers = ('sys/stat.h', 'assert.h', 'kpathsea/kpathsea.h', 'Python.h')
	for i in headers:
		if conf.CheckCHeader (i):
			key = re.sub ('[./]', '_', 'HAVE_' + string.upper (i))
			conf.env['DEFINES'][key] = 1

	ccheaders = ('sstream',)
	for i in ccheaders:
		if conf.CheckCXXHeader (i):
			key = re.sub ('[./]', '_', 'HAVE_' + string.upper (i))
			conf.env['DEFINES'][key] = 1

	functions = ('gettext', 'isinf', 'memmem', 'snprintf', 'vsnprintf')
	for i in functions:
		if 0 or conf.CheckFunc (i):
			key = re.sub ('[./]', '_', 'HAVE_' + string.upper (i))
			conf.env['DEFINES'][key] = 1

	if conf.CheckYYCurrentBuffer ():
		conf.env['DEFINES']['HAVE_FLEXLEXER_YY_CURRENT_BUFFER'] = 1

	if conf.CheckLib ('dl'):
		pass

	if conf.CheckLib ('kpathsea'):
		conf.env['DEFINES']['KPATHSEA'] = 1

	# huh? 
	if conf.CheckLib ('kpathsea', 'kpse_find_file'):
		conf.env['DEFINES']['HAVE_KPSE_FIND_FILE'] = '1'
	if conf.CheckLib ('kpathsea', 'kpse_find_tfm'):
		conf.env['DEFINES']['HAVE_KPSE_FIND_TFM'] = '1'

	#this could happen after flower...
	env.ParseConfig ('guile-config compile')

	#this could happen only for compiling pango-*
	if env['gui']:
		env.ParseConfig ('pkg-config --cflags --libs gtk+-2.0')
		env.ParseConfig ('pkg-config --cflags --libs pango')
		if conf.CheckCHeader ('pango/pangofc-fontmap.h'):
			conf.env['DEFINES']['HAVE_PANGO_PANGOFC_FONTMAP_H'] = '1'

		if conf.CheckLib ('pango-1.0',
				  'pango_fc_font_map_add_decoder_find_func'):
			conf.env['DEFINES']['HAVE_PANGO_CVS'] = '1'
			conf.env['DEFINES']['HAVE_PANGO_FC_FONT_MAP_ADD_DECODER_FIND_FUNC'] = '1'

	if required:
		print
		print '********************************'
		print 'Please install required packages'
		for i in required:
			print '%s:	%s-%s or newer (found: %s %s)' % i
		Exit (1)

	if optional:
		print
		print '*************************************'
		print 'Consider installing optional packages'
		for i in optional:
			print '%s:	%s-%s or newer (found: %s %s)' % i

	return conf.Finish ()

def config_header (target, source, env):
	config = open (str (target[0]), 'w')
	for i in list_sort (env['DEFINES'].keys ()):
		config.write ('#define %s %s\n' % (i, env['DEFINES'][i]))
	config.close ()
env.Command (config_hh, config_cache, config_header)

# hmm?
def xuniquify (lst):
	n = []
	for i in lst:
		if not i in n:
			n.append (i)
	lst = n
	return lst

def uniquify (lst):
	d = {}
	n = len (lst)
	i = 0
	while i < n:
		if not d.has_key (lst[i]):
			d[lst[i]] = 1
			i += 1
		else:
			del lst[i]
			n -= 1
	return lst

def uniquify_config_vars (env):
	for i in config_vars:
		if env.has_key (i) and type (env[i]) == type ([]):
			env[i] = uniquify (env[i])

def save_config_cache (env):
	## FIXME: Is this smart, using option cache for saving
	## config.cache?  I cannot seem to find the official method.
	uniquify_config_vars (env)
	opts.Save (config_cache, env)

	if 'config' in COMMAND_LINE_TARGETS:
		sys.stdout.write ('\n')
		sys.stdout.write ('LilyPond configured')
		sys.stdout.write ('\n')
		sys.stdout.write ('now run')
		sys.stdout.write ('\n')
		sys.stdout.write ('    scons [TARGET|DIR]...')
		sys.stdout.write ('\n')
		Exit (0)
	elif not env['checksums']:
		# When using timestams, config.hh is NEW.  The next
		# build triggers recompilation of everything.  Exiting
		# here makes SCons use the actual timestamp for config.hh
		# and prevents recompiling everything the next run.
		command = sys.argv[0] + ' ' + string.join (COMMAND_LINE_TARGETS)
		sys.stdout.write ('Running %s ... ' % command)
		sys.stdout.write ('\n')
		s = os.system (command)
		Exit (s)


if os.path.exists (config_cache) and 'config' in COMMAND_LINE_TARGETS:
	os.unlink (config_cache)
# WTF?
# scons: *** Calling Configure from Builders is not supported.
# env.Command (config_cache, None, configure)
if not os.path.exists (config_cache) \
   or (os.stat ('SConstruct')[stat.ST_MTIME]
       > os.stat (config_cache)[stat.ST_MTIME]):
	env = configure (None, None, env)
	save_config_cache (env)
elif env['checksums']:
	# just save everything
	save_config_cache (env)

#urg how does #/ subst work?
Export ('env')
SConscript ('buildscripts/builder.py')

env.PrependENVPath ('PATH',
		    os.path.join (env['absbuild'], env['out'], 'usr/bin'))

if os.environ.has_key ('TEXMF'):
	env.Append (ENV = {'TEXMF' : os.environ['TEXMF']})
env.Append (ENV = {
	'TEXMF' : '{$LILYPONDPREFIX,' \
	+ os.popen ('kpsexpand \$TEXMF').read ()[:-1] + '}',
	'LILYPONDPREFIX' : os.path.join (run_prefix, 'share/lilypond'),
	})

BUILD_ABC2LY = '${set__x}$PYTHON $srcdir/scripts/abc2ly.py'
BUILD_LILYPOND = '${set__x}$PYTHON $srcdir/scripts/lilypond.py${__verbose}'
BUILD_LILYPOND_BIN = '$absbuild/$out/lilypond-bin ${__verbose}'
BUILD_LILYPOND_BOOK = '$PYTHON $srcdir/scripts/lilypond-book.py --verbose'


# post-option environment-update
env.Append (
	bindir = bindir,
	sharedir = sharedir,
	lilypond_datadir = sharedir_package,
	localedir = localedir,
	local_lilypond_datadir = sharedir_package_version,
	lilypondprefix = lilypondprefix,
	sharedir_package = sharedir_package,
	sharedir_doc_package = sharedir_doc_package,
	sharedir_package_version = sharedir_package_version,

	LILYPOND = BUILD_LILYPOND,
	ABC2LY = BUILD_ABC2LY,
	LILYPOND_BOOK = BUILD_LILYPOND_BOOK,
	LILYPOND_BOOK_FORMAT = 'texi-html',
	MAKEINFO_FLAGS = '--css-include=$srcdir/Documentation/texinfo.css',

	TEXI2DVI_PAPERSIZE = '@afourpaper',
	TEXI2DVI_FLAGS = [ '-t $TEXI2DVI_PAPERSIZE'],
	DVIPS_PAPERSIZE = 'a4',
	DVIPS_FLAGS = ['-t $DVIPS_PAPERSIZE',
		       '-u+lilypond.map',
		       '-u+ec-mftrace.map'],
	PSPDF_FLAGS = ['-sPAPERSIZE=$DVIPS_PAPERSIZE'],
	)

if env['debugging']:
	env.Append (CCFLAGS = ['-g', '-pipe'])
if env['optimising']:
	env.Append (CCFLAGS = '-O2')
	env.Append (CXXFLAGS = ['-DSTRING_UTILS_INLINED'])
if env['warnings']:
	env.Append (CCFLAGS = ['-W', '-Wall'])
	env.Append (CXXFLAGS = ['-Wconversion'])

# ugr,huh?
env.Append (LINKFLAGS = ['-Wl,--export-dynamic'])

if env['verbose']:
	env['__verbose'] = ' --verbose'
	env['set__x'] = 'set -x;'


## Explicit target and dependencies

if 'clean' in COMMAND_LINE_TARGETS:
	# ugh: prevent reconfigure instead of clean
	os.system ('touch %s' % config_cache)
	
	command = sys.argv[0] + ' -c .'
	sys.stdout.write ('Running %s ... ' % command)
	sys.stdout.write ('\n')
	s = os.system (command)
	if os.path.exists (config_cache):
		os.unlink (config_cache)
	Exit (s)

if 'realclean' in COMMAND_LINE_TARGETS:
	command = 'rm -rf $(find . -name "out-scons" -o -name ".scon*")'
	sys.stdout.write ('Running %s ... ' % command)
	sys.stdout.write ('\n')
	s = os.system (command)
	if os.path.exists (config_cache):
		os.unlink (config_cache)
	Exit (s)

# Declare SConscript phonies 
env.Alias ('minimal', config_cache)
env.Alias ('mf-essential', config_cache)

env.Alias ('minimal', ['lily', 'mf-essential'])
env.Alias ('all', ['minimal', 'mf', '.'])
# Do we want the doc/web separation?
env.Alias ('doc',
	   ['Documentation',
	    'Documentation/user',
	    'Documentation/topdocs',
	    'Documentation/bibliography',
	    'input'])

# Without target arguments, do minimal build
if not COMMAND_LINE_TARGETS:
	env.Default (['minimal'])

# GNU Make rerouting compat:
env.Alias ('web', 'doc')


env.Command (version_hh, '#/VERSION',
	     '$PYTHON ./stepmake/bin/make-version.py VERSION > $TARGET')

# post-config environment update
env.Append (
	run_prefix = run_prefix,
	LILYPONDPREFIX = os.path.join (run_prefix, 'share/lilypond'),

	LIBPATH = [os.path.join (absbuild, 'flower', env['out']),],
	##CPPPATH = [outdir, '#',], # do not read auto*'s header
	CPPPATH = [outdir, ],
	LILYPOND_PATH = ['.', '$srcdir/input',
			 '$srcdir/input/regression',
			 '$srcdir/input/test',
			 '$srcdir/input/tutorial',
			 '$srcdir/Documentation/user',
			 '$absbuild/mf/$out',
#			 os.path.join (absbuild, 'Documentation',
#				       env['out']),
#			 os.path.join (absbuild, 'Documentation/user',
#				       env['out']),
			 ],
	MAKEINFO_PATH = ['.', '$srcdir/Documentation/user',
			 '$absbuild/Documentation/user/$out'],
	)

def symlink_tree (target, source, env):
	def mkdirs (dir):
		def mkdir (dir):
			if not dir:
				os.chdir (os.sep)
				return
			if not os.path.isdir (dir):
				if os.path.exists (dir):
					os.unlink (dir)
				os.mkdir (dir)
			os.chdir (dir)
		map (mkdir, string.split (dir, os.sep))
	def symlink (src, dst):
		os.chdir (absbuild)
		dir = os.path.dirname (dst)
		mkdirs (dir)
		if src[0] == '#':
			frm = os.path.join (srcdir, src[1:])
		else:
			depth = len (string.split (dir, '/'))
			if src.find ('@') > -1:
				frm = os.path.join ('../' * depth,
						    string.replace (src, '@',
								    env['out']))
			else:
				frm = os.path.join ('../' * depth, src,
						    env['out'])
		if src[-1] == '/':
			frm = os.path.join (frm, os.path.basename (dst))
		if env['verbose']:
			print 'ln -s %s -> %s' % (frm, os.path.basename (dst))
		os.symlink (frm, os.path.basename (dst))
	shutil.rmtree (run_prefix)
	prefix = os.path.join (env['out'], 'usr')
	map (lambda x: symlink (x[0], os.path.join (prefix, x[1])),
	     # ^# := source dir
	     # @  := out
	     # /$ := add dst file_name
	     (('python',     'lib/lilypond/python'),
	      ('lily/',      'bin/lilypond-bin'),
	      ('scripts/',   'bin/lilypond'),
	      ('scripts/',   'bin/lilypond-book'),
	      ('mf',         'share/lilypond/dvips'),
	      ('#ps',        'share/lilypond/tex/music-drawing-routines.ps'),
	      ('mf',         'share/lilypond/afm'),
	      ('mf',         'share/lilypond/tfm'),
	      ('#mf',        'share/lilypond/fonts/mf'),
	      ('mf',         'share/lilypond/fonts/afm'),
	      ('mf',         'share/lilypond/fonts/tfm'),
	      ('mf',         'share/lilypond/fonts/type1'),
	      ('#tex',       'share/lilypond/tex/source'),
	      ('mf',         'share/lilypond/tex/generate'),
	      ('#ly',        'share/lilypond/ly'),
	      ('#scm',       'share/lilypond/scm'),
	      ('#ps',        'share/lilypond/ps'),
	      ('po/@/nl.mo', 'share/locale/nl/LC_MESSAGES/lilypond.mo'),
	      ('elisp',      'share/lilypond/elisp')))
	os.chdir (srcdir)

if env['debugging']:
	stamp = os.path.join (run_prefix, 'stamp')
	env.Command (stamp, 'SConstruct', [symlink_tree, 'touch $TARGET'])
	env.Depends ('lily', stamp)
	
#### dist, tar
def plus (a, b):
	a + b

def cvs_entry_is_dir (line):
	return line[0] == 'D' and line[-2] == '/'

def cvs_entry_is_file (line):
	return line[0] == '/' and line[-2] == '/'

def cvs_dirs (dir):
	ENTRIES = os.path.join (dir, 'CVS/Entries')
	if not os.path.exists (ENTRIES):
		return []
	entries = open (ENTRIES).readlines ()
	dir_entries = filter (cvs_entry_is_dir, entries)
	dirs = map (lambda x: os.path.join (dir, x[2:x[2:].index ('/')+3]),
		    dir_entries)
	return dirs + map (cvs_dirs, dirs)

def cvs_files (dir):
	ENTRIES = os.path.join (dir, 'CVS/Entries')
	entries = open (ENTRIES).readlines ()
	file_entries = filter (cvs_entry_is_file, entries)
	files = map (lambda x: x[1:x[1:].index ('/')+1], file_entries)
	return map (lambda x: os.path.join (dir, x), files)

def flatten (tree, lst):
	if type (tree) == type ([]):
		for i in tree:
			if type (i) == type ([]):
				flatten (i, lst)
			else:
				lst.append (i)
	return lst

subdirs = flatten (cvs_dirs ('.'), [])
readme_files = ['AUTHORS', 'README', 'INSTALL', 'NEWS']
foo = map (lambda x: env.TXT (x + '.txt',
			      os.path.join ('Documentation/topdocs', x)),
	   readme_files)
txt_files = map (lambda x: x + '.txt', readme_files)
src_files = reduce (lambda x, y: x + y, map (cvs_files, subdirs))
tar_base = package.name + '-' + version
tar_name = tar_base + '.tar.gz'
ball_prefix = os.path.join (outdir, tar_base)
tar_ball = os.path.join (outdir, tar_name)

dist_files = src_files + txt_files
ball_files = map (lambda x: os.path.join (ball_prefix, x), dist_files)
map (lambda x: env.Depends (tar_ball, x), ball_files)
map (lambda x: env.Command (os.path.join (ball_prefix, x), x,
			    'ln $SOURCE $TARGET'), dist_files)
tar = env.Command (tar_ball, src_files,
		   ['rm -f $$(find $TARGET.dir -name .sconsign)',
		    'tar czf $TARGET -C $TARGET.dir %s' % tar_base,])
env.Alias ('tar', tar)

dist_ball = os.path.join (package.release_dir, tar_name)
env.Command (dist_ball, tar_ball,
	     'if [ -e $SOURCE -a -e $TARGET ]; then rm $TARGET; fi;' \
	     + 'ln $SOURCE $TARGET')
env.Depends ('dist', dist_ball)
patch_name = os.path.join (outdir, tar_base + '.diff.gz')
patch = env.PATCH (patch_name, tar_ball)
env.Depends (patch_name, dist_ball)
env.Alias ('release', patch)

#### web
web_base = os.path.join (outdir, 'web')
web_ball = web_base + '.tar.gz'
env['footify'] = 'MAILADDRESS=bug-lilypond@gnu.org $PYTHON stepmake/bin/add-html-footer.py --name=lilypond --version=$TOPLEVEL_VERSION'
web_ext = ['.html', '.ly', '.midi', '.pdf', '.png', '.ps.gz', '.txt',]
web_path = '-path "*/$out/*"' + string.join (web_ext, ' -or -path "*/$out/*"')
env['web_path'] = web_path
web_list = os.path.join (outdir, 'weblist')
# compatible make heritits
# fixme: generate in $outdir is cwd/builddir
env.Command (web_list,
	     ## this is correct, but takes > 5min if you have a peder :-)
	     ##'doc',
	     '#/VERSION',
	     ['$PYTHON buildscripts/mutopia-index.py -o examples.html ./',
	      'cd $absbuild && $footify $$(find . -name "*.html" -print)',
	      'cd $absbuild && rm -f $$(find . -name "*.html~" -print)',
	      'cd $absbuild && find Documentation input $web_path \
	      > $TARGET',
	      '''echo '<META HTTP-EQUIV="refresh" content="0;URL=Documentation/out-www/index.html">' > $absbuild/index.html''',
	      '''echo '<html><body>Redirecting to the documentation index...</body></html>' >> $absbuild/index.html''',
	      'cd $absbuild && ls *.html >> $TARGET',])
env.Command (web_ball, web_list,
	     ['cat $SOURCE | tar -C $absbuild -czf $TARGET -T -',])
#env.Alias ('web', web_ball)
www_base = os.path.join (outdir, 'www')
www_ball = www_base + '.tar.gz'
env.Command (www_ball, web_ball,
	     ['rm -rf $out/tmp',
	      'mkdir -p $absbuild/$out/tmp',
	      'tar -C $absbuild/$out/tmp -xzf $SOURCE',
	      'cd $absbuild/$out/tmp && for i in $$(find . -name "$out"); do mv $$i $$(dirname $$i)/out-www; done',
	      'tar -C $absbuild/$out/tmp -czf $TARGET .'])
env.Alias ('web', www_ball)

#### tags
env.Append (
	ETAGSFLAGS = ["""--regex='{c++}/^LY_DEFINE *(\([^,]+\)/\1/'""",
		      """--regex='{c++}/^LY_DEFINE *([^"]*"\([^"]+\)"/\1/'"""])
# filter-out some files?
env.Command ('TAGS', src_files, 'etags $ETAGSFLAGS $SOURCES')


# Note: SConscripts are only needed in directories where something needs
# to be done, building or installing
for d in subdirs:
	if os.path.exists (os.path.join (d, 'SConscript')):
		b = os.path.join (env['build'], d, env['out'])
		# Support clean sourcetree build (--srcdir build)
		# and ./out build.
		if os.path.abspath (b) != os.path.abspath (d):
			env.BuildDir (b, d, duplicate = 0)
       		SConscript (os.path.join (b, 'SConscript'))

