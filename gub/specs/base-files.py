from gub import build
from gub import mirrors

url='ftp://ftp.nl.debian.org/debian/pool/main/b/base-files/base-files_4.0.0.tar.gz'

class Base_files (build.BinaryBuild):
    def __init__ (self, settings, source):
        build.BinaryBuild.__init__ (self, settings, source)
        from gub import repository
        # FIXME: cannot parse debian balls
    source = mirrors.with_tarball (name='base-files', mirror=url, version='', ball_version='4.0.0')
    def get_subpackage_names (self):
        return ['']
    def patch (self):
        fakeroot_cache = self.builddir () + '/fakeroot.cache'
        self.fakeroot (self.expand (self.settings.fakeroot, locals ()))
        self.system ('''
cd %(srcdir)s && mkdir -p `cat debian/directory-list`
cd %(srcdir)s && mkdir -p usr/share/base-files
cd %(srcdir)s && mv share usr/share/base-files
cd %(srcdir)s && mv licenses usr/share/common-licenses
# from debian/rules
cd %(srcdir)s && chown root:src     usr/src
cd %(srcdir)s && chown root:staff   var/local
cd %(srcdir)s && chmod 755  `find . -type d`
cd %(srcdir)s && chmod 1777 `cat debian/1777-dirs`
cd %(srcdir)s && chmod 2775 `cat debian/2775-dirs`
cd %(srcdir)s && rm -rf debian
''')
    def xinstall (self):
        self.system ('''
cd %(install-root)s && chown root:src     usr/src
cd %(install_root)s && chown root:staff   var/local
''')