# -*-python-*-

Import ('env')

env.Append (
    TEXINFO_PATH = [],
    TEXI2HTML_INCPREFIX = '-I=', # Weird nonstandard include prefix
    TEXINFO_INCFLAGS = '$( ${_concat(TEXI2HTML_INCPREFIX, TEXINFO_PATH, INCSUFFIX, __env__, RDirs)} $)',
    TEXI2HTML_FLAGS = ['--init-file=web-texi2html.init', '--split=section'],
    )

texi2html_command = 'texi2html $TEXI2HTML_FLAGS $TEXINFO_INCFLAGS --output=$OUT '
texiBuild = Builder (action = texi2html_command + ' $SOURCE ')
env.Append (BUILDERS = {'texiBuild' : texiBuild})

Export ('env')
