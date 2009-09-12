srcdir = '.'
blddir = 'out'

from configobj import ConfigObj

import subprocess
from subprocess import *

import os


def checkfile(file):
    filecheck = \
        Popen(["ls", file], stderr=PIPE, stdout=PIPE).communicate()
    return (filecheck[1] == '')

def configure(conf):
    env_lilypond_binary = os.getenv('LILYPOND_EXTERNAL_BINARY')
    if not(env_lilypond_binary):
        if filecheck(os.path.join(blddir,'GNUmakefile')) and \
           filecheck(os.path.join(blddir,'config.make)) and \
           Popen(["tail", "-1", os.path.join(blddir,'config.log')).communicate()[0] == \
              'configure: exit 0':
            env_lilypond_binary = "$(configure-builddir)/out/bin/lilypond --relocate"
    else:
        env_lilypond_binary = conf.check_program('lilypond', var='LILYPOND_BINARY')
    conf.env.append_value('LILYPOND_BINARY', env_lilypond_binary)

config = ConfigObj['VERSION']
APPNAME = config['PACKAGE_NAME']
VERSION = (config['MAJOR_VERSION'] + '.' +
           config['MINOR_VERSION'] + '.' +
           config['PATCH_LEVEL'])


