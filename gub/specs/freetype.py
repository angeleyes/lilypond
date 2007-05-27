from gub import mirrors
from gub import gubb
from gub import targetpackage
from gub import toolpackage

class Freetype (targetpackage.TargetBuildSpec):
    def __init__ (self, settings):
        targetpackage.TargetBuildSpec.__init__ (self, settings)
        self.with_template (version='2.1.10', mirror=mirrors.nongnu_savannah)

    def license_file (self):
        return '%(srcdir)s/docs/LICENSE.TXT'

    def get_build_dependencies (self):
        return ['libtool-devel', 'zlib-devel']

    def get_subpackage_names (self):
        return ['devel', '']

    def get_dependency_dict (self):
        return {'': ['zlib']}

    def configure (self):
#                self.autoupdate (autodir=os.path.join (self.srcdir (),
#                                                       'builds/unix'))

        self.system ('''
        rm -f %(srcdir)s/builds/unix/{unix-def.mk,unix-cc.mk,ftconfig.h,freetype-config,freetype2.pc,config.status,config.log}
''')
        targetpackage.TargetBuildSpec.configure (self)

        ## FIXME: libtool too old for cross compile
        self.update_libtool ()

        self.file_sub ([('^LIBTOOL=.*', 'LIBTOOL=%(builddir)s/libtool --tag=CXX')], '%(builddir)s/Makefile')

    def munge_ft_config (self, file):
        self.file_sub ([('\nprefix=[^\n]+\n',
                         '\nlocal_prefix=yes\nprefix=%(system_root)s/usr\n')],
                       file, must_succeed=True)

    def install (self):
        targetpackage.TargetBuildSpec.install (self)
        self.system ('mkdir -p %(install_root)s/usr/cross/bin/')
        self.system ('mv %(install_root)s/usr/bin/freetype-config %(install_root)s/usr/cross/bin/freetype-config')
        self.munge_ft_config ('%(install_root)s/usr/cross/bin/freetype-config')

class Freetype__mingw (Freetype):
    def configure (self):
        Freetype.configure (self)
        self.dump ('''
# libtool will not build dll if -no-undefined flag is not present
LDFLAGS:=$(LDFLAGS) -no-undefined
''',
             '%(builddir)s/Makefile',
             mode='a')

class Freetype__local (toolpackage.ToolBuildSpec, Freetype):
    def __init__ (self, settings):
        toolpackage.ToolBuildSpec.__init__ (self, settings)
        self.with_template (version='2.1.10', mirror=mirrors.nongnu_savannah)

    def get_build_dependencies (self):
        # local is not split
        #return ['libtool-devel']
        return ['libtool']

    # FIXME, mi-urg?
    def license_file (self):
        return Freetype.license_file (self)

    def install (self):
        toolpackage.ToolBuildSpec.install (self)
        self.munge_ft_config ('%(install_root)s/%(local_prefix)s/bin/.freetype-config')