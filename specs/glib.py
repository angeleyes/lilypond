import download
import toolpackage
import targetpackage

class Glib (targetpackage.TargetBuildSpec):
    def __init__ (self, settings):
        targetpackage.TargetBuildSpec.__init__ (self, settings)


        ## 2.12.4 : see bug  http://bugzilla.gnome.org/show_bug.cgi?id=362918
        self.with (#version='2.12.4',   mirror=download.gnome_216,
            version='2.10.3',
		   mirror=download.gnome_214,
		   format='bz2')

    def get_build_dependencies (self):
        return ['gettext-devel', 'libtool']

    def get_dependency_dict (self):
        d = targetpackage.TargetBuildSpec.get_dependency_dict (self)
        d[''].append ('gettext')
        return d
    
    def config_cache_overrides (self, str):
        return str + '''
glib_cv_stack_grows=${glib_cv_stack_grows=no}
'''
    def configure (self):
        targetpackage.TargetBuildSpec.configure (self)

        ## FIXME: libtool too old for cross compile
        self.update_libtool ()
        
    def install (self):
        targetpackage.TargetBuildSpec.install (self)
        self.system ('rm %(install_root)s/usr/lib/charset.alias',
                     ignore_errors=True)
        
class Glib__darwin (Glib):
    def configure (self):
        Glib.configure (self)
        self.file_sub ([('nmedit', '%(target_architecture)s-nmedit')],
                       '%(builddir)s/libtool')
        
class Glib__mingw (Glib):
    def get_dependency_dict (self):
        d = Glib.get_dependency_dict (self)
        d[''].append ('libiconv')
        return d
    
    def get_build_dependencies (self):
        return Glib.get_build_dependencies (self) + ['libiconv-devel']

class Glib__freebsd (Glib):
    def get_dependency_dict (self):
        d = Glib.get_dependency_dict (self)
        d[''].append ('libiconv')
        return d
    
    def get_build_dependencies (self):
        return Glib.get_build_dependencies (self) + ['libiconv-devel']
    
    def configure_command (self):
        return Glib.configure_command (self) + ' --disable-threads'
        
class Glib__local (toolpackage.ToolBuildSpec):
    def __init__ (self, settings):
        toolpackage.ToolBuildSpec.__init__ (self, settings)
        self.with (version='2.10.3',
                   mirror=download.gnome_214,
                   format='bz2')

    def install (self):
        toolpackage.ToolBuildSpec.install(self)
        self.system ('rm %(install_root)s/%(packaging_suffix_dir)s/usr/lib/charset.alias',
                         ignore_errors=True)

    def get_build_dependencies (self):
        return ['gettext-devel', 'libtool']            