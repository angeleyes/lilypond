# -*-python-*-

# FIXME: this is undocumented cruft, apparently needed?
# http://www.nabble.com/site_scons----site_init.py-issues-tt20447579.html#a20447579
from SCons.Builder import *
 
def add_texi2html_builder (env):
    env.Append (TEXINFO_PATH = [],
                TEXI2HTML_INCPREFIX = '-I=', # FIXME: texi2html uses a
                                             # weird nonstandard
                                             # include prefix
                TEXINFO_INCFLAGS = '$( ${_concat(TEXI2HTML_INCPREFIX, TEXINFO_PATH, INCSUFFIX, __env__, RDirs)} $)',
                TEXI2HTML_FLAGS = ['--init-file=web-texi2html.init', '--split=section'],)
    texi2html_command = 'texi2html $TEXI2HTML_FLAGS $TEXINFO_INCFLAGS --output=$OUT '
    texiBuild = Builder (action=texi2html_command + ' $SOURCE ')
    env.Append (BUILDERS={'texiBuild' : texiBuild})
