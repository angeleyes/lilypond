# -*-python-*-

import glob
import os
import string

Import ('env')

# utility

def add_suffixes (target, source, env, target_suffixes, src_suffixes):
	base = os.path.splitext (str (target[0]))[0]
	return (target + map (lambda x: base + x, target_suffixes),
		source + map (lambda x: base + x, src_suffixes))

# junkme; see _concat
def join_path (path, infix=os.pathsep, prefix = ''):
	def dir (x):
		if x and x[0] == '#':
			return env['srcdir'] + x[1:]
		return x
	return string.join (map (lambda x: prefix + dir (x), path), infix)


def src_glob (s):
	here = os.getcwd ()
	os.chdir (env.Dir ('.').srcnode ().abspath)
	result = glob.glob (s)
	os.chdir (here)
	return result

Export ('src_glob')

def base_glob (s):
	return map (lambda x: os.path.splitext (x)[0], src_glob (s))

Export ('base_glob')

def install (target, dir):
	dest = env['DESTDIR'] + dir
	if type (target) == type ([]):
		map (lambda x: env.Install (dir, x), target)
	else:
		env.Install (dir, target)
	env.Alias ('install', dir)

Export ('install')

def _fixme (s):
	x = string.replace (s, '#', env['srcdir'])
	x = string.replace (x, '@', env['absbuild'])
	return x

# Clean separation between generic action + flags and actual
# configuration and flags in environment for this build.

# Generic builders could/should be part of SCons.


HH = Builder (action = 'bison -d -o ${TARGET.base}.cc $SOURCE',
	      suffix = '.hh', src_suffix = '.yy')
env.Append (BUILDERS = {'HH' : HH})


# Setup LilyPond environment.  For the LilyPond build, we override
# some of these commands in the ENVironment.

env.Append (
	_fixme = _fixme,
	ABC2LY = 'abc2ly',
	LILYPOND = 'lilypond',
	LILYPOND_BIN = 'lilypond-bin',
	LILYOND_BOOK = 'lilypond-book',

	#ugr
	#LILYPOND_BOOK_FORMAT = 'texi',
	LILYPOND_BOOK_FORMAT = '',
	LILYPOND_BOOK_FLAGS = ['--format=$LILYPOND_BOOK_FORMAT'],

	LILYPOND_PATH = [],
	# The SCons way around FOO_PATH:
	##LILYPOND_INCFLAGS = '$( ${_concat(INCPREFIX, LILYPOND_PATH, INCSUFFIX, __env__, RDirs)} $)',
	LILYPOND_INCFLAGS = '$( ${_concat(INCPREFIX, LILYPOND_PATH, INCSUFFIX, __env__)} $)',

	MAKEINFO_PATH = [],
	MAKEINFO_FLAGS = [],
	MAKEINFO_INCFLAGS = '$( ${_concat(INCPREFIX, MAKEINFO_PATH, INCSUFFIX, __env__, RDirs)} $)',

	TEXI2DVI_FLAGS = [],
	_TEXI2DVI_FLAGS = '$( ${_concat(" ", TEXI2DVI_FLAGS,)} $)',
	)

TXT =\
    Builder (action = '$MAKEINFO --output=$TARGET $MAKEINFO_INCFLAGS\
    --no-split --no-headers $SOURCE',
	       suffix = '.txt', src_suffix = '.texi')
env.Append (BUILDERS = {'TXT': TXT})

INFO =\
     Builder (action = '$MAKEINFO --output=$TARGET $MAKEINFO_INCFLAGS $SOURCE',
	      suffix = '.info', src_suffix = '.texi')
env.Append (BUILDERS = {'INFO': INFO})

HTML =\
     Builder (action = '$MAKEINFO --output=$TARGET $MAKEINFO_INCLUDES\
     --html --no-split --no-headers $MAKEINFO_FLAGS $SOURCE',
suffix = '.html', src_suffix = '.texi')
env.Append (BUILDERS = {'HTML': HTML})

TEXI =\
     Builder (action =
	      '$LILYPOND_BOOK --output=${TARGET.dir} \
	      --include=${TARGET.dir} $LILYPOND_INCFLAGS \
	      --process="$LILYPOND_BIN $LILYPOND_INCFLAGS" \
	      $LILYPOND_BOOK_FLAGS \
	      $SOURCE',
	      suffix = '.texi', src_suffix = '.tely')
env.Append (BUILDERS = {'TEXI': TEXI})

TEXIDVI =\
	Builder (action = 'cd ${TARGET.dir} && \
	texi2dvi --batch $_TEXI2DVI_FLAGS ${SOURCE.file}',
		 suffix = '.dvi', src_suffix = '.texi')
env.Append (BUILDERS = {'TEXIDVI': TEXIDVI})

DVIPS =\
      Builder (action = 'dvips -o $TARGET $DVIPS_FLAGS $SOURCE',
	       suffix = '.ps', src_suffix = '.dvi')
env.Append (BUILDERS = {'DVIPS': DVIPS})

DVIPDF =\
      Builder (action = 'dvips -o $TARGET -Ppdf $DVIPS_FLAGS $SOURCE',
	       suffix = '.pdfps', src_suffix = '.dvi')
env.Append (BUILDERS = {'DVIPDF': DVIPDF})

PSPDF =\
      Builder (action = 'ps2pdf $PSPDF_FLAGS $SOURCE $TARGET',
	       suffix = '.pdf', src_suffix = '.pdfps')
env.Append (BUILDERS = {'PSPDF': PSPDF})

PNG2EPS =\
	Builder (action = 'convert $SOURCE $TARGET',
		 suffix = '.eps', src_suffix = '.png')
env.Append (BUILDERS = {'PNG2EPS': PNG2EPS})





# FIXME: cleanup, see above


env.Append (

	#urg
	BSTINPUTS = '${SOURCE.dir}:${TARGET.dir}:',
	BIB2HTML = '$PYTHON $srcdir/buildscripts/bib2html.py',
)


def add_ps_target (target, source, env):
	base = os.path.splitext (str (target[0]))[0]
	return (target + [base + '.ps'], source)

# TODO: 
# FIXME: INCLUDES, FLAGS, use LILYPOND_BIN for building ? 
lilypond =\
	 Builder (action = '$LILYPOND --output=${TARGET.base} --include=${TARGET.dir} $SOURCE',
		  suffix = '.pdf', src_suffix = '.ly')
##		    emitter = add_ps_target)
env.Append (BUILDERS = {'LilyPond': lilypond})

ABC = Builder (action = '$ABC2LY --output=${TARGET} --strict $SOURCE',
	       suffix = '.ly', src_suffix = '.abc')
env.Append (BUILDERS = {'ABC': ABC})

def add_log_target (target, source, env):
	base = os.path.splitext (str (target[0]))[0]
	return (target + [base + '.log'], source)

def add_enc_ly_tex_target (target, source, env):
	base = os.path.splitext (str (target[0]))[0]
	return (target + [base + '.enc', base + '.tex', base + 'list.ly'],
		source)
a = 'cd ${TARGET.dir} && \
MFINPUTS=.:${SOURCE.dir}:$srcdir/${SOURCE.dir}: \
mf "\\mode:=$MFMODE; nonstopmode; input ${SOURCE.filebase};" \
| grep -v "@\|>>"'
tfm = Builder (action = a, suffix = '.tfm', src_suffix = '.mf',
#	       emitter = lambda t, s, e: add_suffixes (t, s, e, ['.log'], []))
	       emitter = add_log_target)
env.Append (BUILDERS = {'TFM': tfm})

a = '$PYTHON $MF_TO_TABLE_PY \
--outdir=${TARGET.dir} \
--afm=${TARGET.base}.afm \
--enc=${TARGET.base}.enc \
--tex=${TARGET.base}.tex \
--ly=${TARGET.base}list.ly \
${TARGET.base}.log'
afm = Builder (action = a, suffix = '.afm', src_suffix = '.log',
	       emitter = add_enc_ly_tex_target)
env.Append (BUILDERS = {'AFM': afm})

def add_enc_src (target, source, env):
	base = os.path.splitext (str (target[0]))[0]
	return (target, source + [base + '.enc'])

# UGH, should fix --output option for mftrace
# mftrace --verbose is too verbose
a = 'cd ${TARGET.dir} && \
if test -e ${SOURCE.filebase}.enc; then encoding="--encoding=${SOURCE.filebase}.enc"; fi; \
MFINPUTS=$srcdir/mf:.: \
mftrace --pfa --simplify --keep-trying $$encoding $TOO__verbose \
--include=${TARGET.dir} \
${SOURCE.file}'

pfa = Builder (action = a,
	       suffix = '.pfa',
	       src_suffix = '.mf',
	       emitter = add_enc_src)
env.Append (BUILDERS = {'PFA': pfa})

# Specific builders
env['DIFF_PY'] = '$srcdir/stepmake/bin/package-diff.py'
a = '$PYTHON $DIFF_PY $__verbose --outdir=${TARGET.dir}'
patch = Builder (action = a, suffix = '.diff', src_suffix = '.tar.gz')
env.Append (BUILDERS = {'PATCH': patch})

atvars = [
'BASH',
'DATE',
'sharedstatedir',
'GUILE',
'bindir',
'date',
'datadir',
'lilypond_datadir',
'lilypond_libdir',
'local_lilypond_datadir',
'local_lilypond_libdir',
'localedir',
'PACKAGE',
'package',
'PATHSEP',
'PERL',
'prefix',
'program_prefix',
'program_suffix',
'PYTHON',
'SHELL',
'TOPLEVEL_VERSION',
'step-bindir',
]

# naming
def at_copy (target, source, env):
    s = open (str (source[0])).read ()
    for i in atvars:
	    if env.has_key (i):
		    s = string.replace (s, '@%s@'% i, env[i])
    t = str (target[0])
    open (t, 'w').write (s)
    # wugh
    if os.path.basename (os.path.dirname (str (target[0]))) == 'bin':
	    os.chmod (t, 0755)

AT_COPY = Builder (action = at_copy, src_suffix = ['.in', '.py', '.sh',])
env.Append (BUILDERS = {'AT_COPY': AT_COPY})

MO = Builder (action = 'msgfmt -o $TARGET $SOURCE',
	      suffix = '.mo', src_suffix = '.po')
env.Append (BUILDERS = {'MO': MO})

ugh =  'ln -f po/lilypond.pot ${TARGET.dir}/lilypond.po; '
a = ugh + 'xgettext --default-domain=lilypond --join \
--output-dir=${TARGET.dir} --add-comments \
--keyword=_ --keyword=_f --keyword=_i $SOURCES'
PO = Builder (action = a, suffix = '.pot',
	      src_suffix = ['.cc', '.hh', '.py'], multi = 1)
env['potarget'] = os.path.join (env['absbuild'], 'po', env['out'],
				'lilypond.pot')
env['pocommand'] = a

ugh = '; mv ${TARGET} ${SOURCE}'
a = 'msgmerge ${SOURCE} ${SOURCE.dir}/lilypond.pot -o ${TARGET}' + ugh
POMERGE = Builder (action = a, suffix = '.pom', src_suffix = '.po')
env.Append (BUILDERS = {'POMERGE': POMERGE})

#UGRr
a = 'BSTINPUTS=$BSTINPUTS $BIB2HTML -o $TARGET $SOURCE'
BIB2HTML = Builder (action = a, suffix = '.html', src_suffix = '.bib')
env.Append (BUILDERS = {'BIB2HTML': BIB2HTML})

a = '$PYTHON $srcdir/buildscripts/lys-to-tely.py \
--name=${TARGET.base} --title="$TITLE" $SOURCES'
LYS2TELY = Builder (action = a, suffix = '.tely', src_suffix = '.ly')
env.Append (BUILDERS = {'LYS2TELY': LYS2TELY})


def mutopia (ly = None, abc = None):

	# FIXME: ugr, huh?  The values from ../SConstruct get appended
	# to the predefined values from this builder context:

	# abc2ly/usr/bin/python ..../abc2.py

	# Override them again to fix web build...

	
	BUILD_ABC2LY = '${set__x}$PYTHON $srcdir/scripts/abc2ly.py'
	BUILD_LILYPOND = '${set__x}$PYTHON $srcdir/scripts/lilypond.py${__verbose}'
	e = env.Copy (
		LILYPOND = BUILD_LILYPOND,
		ABC2LY = BUILD_ABC2LY,
	)
	
	if not abc:
		abc = base_glob ('*.abc')
	if not ly:
		ly = base_glob ('*.ly') + map (e.ABC, abc)
	pdf = map (e.LilyPond, ly)
	
	# We need lily and mf to build these.
	env.Depends (pdf, ['#/lily', '#/mf'])
	env.Alias ('doc', pdf)

Export ('mutopia')


def collate (title = 'collated files'):
	ly = base_glob ('*.ly')
	
	e = env.Copy (
		TITLE = title,
		LILYPOND_BOOK_FLAGS = '''--process="lilypond-bin --header=texidoc -I$srcdir/input/test -e '(ly:set-option (quote internal-type-checking) #t)'"''',
													    __verbose = ' --verbose',
													    )
	#		
	tely = e.LYS2TELY ('collated-files', ly)
	texi = e.TEXI (tely)
	# We need lily and mf to build these.
	env.Depends (texi, ['#/lily', '#/mf'])
	dvi = e.TEXIDVI (texi)
	pspdf = e.DVIPDF (dvi)
	pdf = e.PSPDF (pspdf)
	html = e.HTML (texi)

	env.Alias ('doc', pdf)
	env.Alias ('doc', html)

Export ('collate')

Export ('env')
