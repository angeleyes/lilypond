# -*- mode: python; encoding: utf-8 -*-
srcdir = '.'
blddir = 'out'

import os
import sys

import pproc
import Options

try:
    srcdir = Options.options.srcdir
except:
    pass
try:
    blddir = Options.options.blddir
except:
    pass

custom_modules_dir = os.path.join (srcdir, 'python', 'auxiliar')
sys.path.append (custom_modules_dir)
custom_waf_tools_dir = os.path.join (custom_modules_dir, 'waf-tools')

import custom_configure
custom_configure.outdir = 'out/conf'

from configobj import ConfigObj

config = ConfigObj('VERSION')
APPNAME = config['PACKAGE_NAME']
VERSION = (config['MAJOR_VERSION'] + '.' +
           config['MINOR_VERSION'] + '.' +
           config['PATCH_LEVEL'])

def configure(conf):
    conf.env['top-src-dir'] = srcdir
    conf.env['top-build-dir'] = blddir

    conf.check_tool ('texinfo', tooldir=custom_waf_tools_dir)
    conf.find_program_with_version ('makeinfo', '4.11')
    conf.find_program_with_version (['texi2html', 'texi2html.pl'], '1.82')

    conf.check_tool ('lilypond', tooldir=custom_waf_tools_dir)
