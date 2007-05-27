from gub import targetpackage
from gub import mirrors

class Libiconv (targetpackage.TargetBuildSpec):
    def __init__ (self, settings):
        targetpackage.TargetBuildSpec.__init__ (self, settings)
        self.with_template (version='1.11', mirror=mirrors.gnu)
    
    def broken_for_distcc (self):
        return True
    
    def get_build_dependencies (self):
        return ['gettext-devel', 'libtool']

    def configure (self):
        targetpackage.TargetBuildSpec.configure (self)
        # # FIXME: libtool too old for cross compile
        self.update_libtool ()
        
    def install (self):
        targetpackage.TargetBuildSpec.install (self)
        self.system ('rm %(install_root)s/usr/lib/charset.alias')

    def license_file (self):
        return '%(srcdir)s/COPYING.LIB'