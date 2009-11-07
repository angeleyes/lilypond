# -*- mode: python; encoding: utf-8 -*-
srcdir = '.'
blddir = 'out'

import os
import sys

import pproc

custom_modules_dir = os.path.join (srcdir, 'build', 'waf')
sys.path.append (custom_modules_dir)
custom_waf_tools_dir = os.path.join (custom_modules_dir, 'waf-tools')


def configure(conf):
    import custom_configure
    custom_configure.outdir = os.path.join (conf.blddir, 'conf')

    from configobj import ConfigObj

    config = ConfigObj('VERSION')
    APPNAME = config['PACKAGE_NAME']
    VERSION = (config['MAJOR_VERSION'] + '.' +
               config['MINOR_VERSION'] + '.' +
               config['PATCH_LEVEL'])

    conf.env['top-src-dir'] = conf.srcdir
    conf.env['top-build-dir'] = conf.blddir

    conf.check_tool ('texinfo', tooldir=custom_waf_tools_dir)
    conf.find_program_with_version ('makeinfo', '4.11')
    conf.find_program_with_version (['texi2html', 'texi2html.pl'], '1.82')

    conf.check_tool ('lilypond', tooldir=custom_waf_tools_dir)
