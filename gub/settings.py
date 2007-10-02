import os
import re
from gub import oslog
from gub import distcc
from gub import build
from gub import context
from gub import mirrors
from gub import misc

platforms = {
    'debian': 'i686-linux',
    'debian-arm': 'arm-linux',
    'debian-mipsel': 'mipsel-linux',
    'debian-x86': 'i686-linux',
    'cygwin': 'i686-cygwin',
    'darwin-ppc': 'powerpc-apple-darwin7',
    'darwin-x86': 'i686-apple-darwin8',

    'freebsd4-x86': 'i686-freebsd4',
    'freebsd6-x86': 'i686-freebsd6',

    'freebsd-x86': 'i686-freebsd4',
    'freebsd-64': 'x86_64-freebsd6',

    'linux-arm': 'arm-linux',
    'linux-arm-softfloat': 'armv5te-softfloat-linux',
    'linux-arm-vfp': 'arm-linux',

    'linux-mipsel': 'mipsel-linux',

    'linux-x86': 'i686-linux',
    'linux-64': 'x86_64-linux',
    'linux-ppc': 'powerpc-linux',
    'tools': 'tools',
    'mingw': 'i686-mingw32',
}

distros = ('cygwin')

class UnknownPlatform (Exception):
    pass

def get_platform_from_dir (settings, dir):
    m = re.match ('.*?([^/]+)(' + settings.root_dir + ')?/*$', dir)
    if m:
        return m.group (1)
    return None

class Settings (context.Context):
    def __init__ (self, options, os_interface=None):
        context.Context.__init__ (self)

        # TODO: tools-prefix, target-prefix, cross-prefix?
        self.prefix_dir = '/usr'
        self.root_dir = '/root'
        self.cross_dir = '/cross'

        if not options.platform:
            if options.__dict__.has_key ('root') and options.root:
                options.platform = get_platform_from_dir (self, options.root)
                if not options.platform:
                    self.error ('invalid root: %(root)s, no platform found'
                                % options)
                    raise UnknownPlatform (options.root)
            else:
                guess = get_platform_from_dir (self, os.getcwd ())
                if guess in platforms.keys ():
                    options.platforms = guess
            
        if not options.platform:
            options.platform = 'tools'

        self.platform = options.platform
        if self.platform not in platforms.keys ():
            raise UnknownPlatform (self.platform)

        GUB_TOOLS_PREFIX = os.environ.get ('GUB_TOOLS_PREFIX')
        
        # config dirs

        if self.platform == 'tools' and GUB_TOOLS_PREFIX:
            self.prefix_dir = ''

        # gubdir is top of `installed' gub repository
        ## self.gubdir = os.getcwd ()
        self.gubdir = os.path.dirname (os.path.dirname (__file__))
        if not self.gubdir:
            self.gubdir = os.getcwd ()

        # workdir is top of writable build stuff
        ##self.workdir = os.getcwd ()
        self.workdir = self.gubdir
        
        # gubdir based: fixed repository layout
        self.patchdir = self.gubdir + '/patches'
        self.sourcefiledir = self.gubdir + '/sourcefiles'
        self.specdir = self.gubdir + '/gub/specs'
        self.nsisdir = self.gubdir + '/nsis'

        # workdir based; may be changed
        self.logdir = self.workdir + '/log'
        self.downloads = self.workdir + '/downloads'
        self.alltargetdir = self.workdir + '/target'
        self.targetdir = self.alltargetdir + '/' + self.platform

        self.system_root = self.targetdir + self.root_dir
        if self.platform == 'tools' and GUB_TOOLS_PREFIX:
            self.system_root = GUB_TOOLS_PREFIX
        self.system_prefix = self.system_root + self.prefix_dir

        ## Patches are architecture dependent, 
        ## so to ensure reproducibility, we unpack for each
        ## architecture separately.
        self.allsrcdir = self.targetdir + '/src'
        self.allbuilddir = self.targetdir + '/build'
        self.statusdir = self.targetdir + '/status'
        self.packages = self.targetdir + '/packages'

        self.uploads = self.workdir + '/uploads'
        self.platform_uploads = self.uploads + '/' + self.platform

        # FIXME: rename to cross_root?
        ##self.cross_prefix = self.system_prefix + self.cross_dir
        self.cross_prefix = self.targetdir + self.root_dir + self.prefix_dir + self.cross_dir
        self.installdir = self.targetdir + '/install'
        self.tools_root = self.alltargetdir + '/tools' + self.root_dir
        self.tools_prefix = self.tools_root + self.prefix_dir
        if GUB_TOOLS_PREFIX:
            self.tools_root = GUB_TOOLS_PREFIX
            self.tools_prefix = GUB_TOOLS_PREFIX
        self.cross_distcc_bindir = self.alltargetdir + '/cross-distcc/bin'
        self.native_distcc_bindir = self.alltargetdir + '/native-distcc/bin'

        self.cross_packages = self.packages + self.cross_dir
        self.cross_allsrcdir = self.allsrcdir + self.cross_dir
        self.cross_statusdir = self.statusdir + self.cross_dir

        self.core_prefix = self.cross_prefix + '/core'
        # end config dirs



        self.target_gcc_flags = '' 
        if self.platform == 'darwin-ppc':
            self.target_gcc_flags = '-D__ppc__'
        elif self.platform == 'mingw':
            self.target_gcc_flags = '-mwindows -mms-bitfields'

        self.branches = dict ()
        if options.branches:
            self.set_branches (options.branches)
        self.options = options ##ugh
        self.verbose = self.options.verbose
        self.os = re.sub ('[-0-9].*', '', self.platform)

        self.target_architecture = platforms[self.platform]
        self.cpu = self.target_architecture.split ('-')[0]
        self.build_source = False
        self.is_distro = (self.platform in distros
                          or self.platform.startswith ('debian'))


        self.gtk_version = '2.8'
        self.toolchain_prefix = self.target_architecture + '-'
        self.distcc_hosts = ''
        
	if self.target_architecture.startswith ('x86_64'):
	    self.package_arch = 'amd64'
            self.debian_branch = 'unstable'
	else:
            self.package_arch = re.sub ('-.*', '', self.target_architecture)
            self.package_arch = re.sub ('i[0-9]86', 'i386', self.package_arch)
            self.package_arch = re.sub ('arm.*', 'arm', self.package_arch)
#            self.package_arch = re.sub ('powerpc.*', 'ppc', self.package_arch)
            self.debian_branch = 'stable'
        
        self.keep_build = False
        self.use_tools = False

        self.fakeroot_cache = '' # %(builddir)s/fakeroot.save'
        self.fakeroot = 'fakeroot -i%(fakeroot_cache)s -s%(fakeroot_cache)s '

        if not os.path.isdir ('log'):
            os.mkdir ('log')
            
        self.os_interface = os_interface
        if not self.os_interface:
            self.os_interface = oslog.Os_commands (('log/%(platform)s.log'
                                                    % self.__dict__),
                                                   self.options.verbose)
        self.create_dirs ()
        self.build_architecture = self.os_interface.read_pipe ('gcc -dumpmachine',
                                                               silent=True)[:-1]

        try:
            self.cpu_count_str = '%d' % os.sysconf ('SC_NPROCESSORS_ONLN')
        except ValueError:
            self.cpu_count_str = '1'

        ## make sure we don't confuse build or target system.
        self.LD_LIBRARY_PATH = '%(system_root)s'
        
    def create_dirs (self): 
        for a in (
            'allsrcdir',
            'core_prefix',
            'cross_prefix',
            'downloads',
            'gubdir',
            'tools_prefix',
            'logdir',
            'packages',
            'specdir',
            'statusdir',
            'system_root',
            'targetdir',
            'uploads',

            'cross_packages',
            'cross_statusdir',
            'cross_allsrcdir',
            ):
            dir = self.__dict__[a]
            if os.path.isdir (dir):
                continue

            self.os_interface.system ('mkdir -p %s' % dir, defer=False)

    def set_distcc_hosts (self, options):
        def hosts (xs):
            return reduce (lambda x,y: x+y,
                           [ h.split (',') for h in xs], [])
        
        self.cross_distcc_hosts = ' '.join (distcc.live_hosts (hosts (options.cross_distcc_hosts)))

        self.native_distcc_hosts = ' '.join (distcc.live_hosts (hosts (options.native_distcc_hosts), port=3634))


    def set_branches (self, bs):
        "set branches, takes a list of name=branch strings."

        self.branch_dict = dict ()
        for b in bs:
            (name, br) = tuple (b.split ('='))
            self.branch_dict[name] = br
            self.__dict__['%s_branch' % name] = br

    def dependency_url (self, string):
        # FIXME: read from settings.rc, take platform into account
        import sources
        name = string.replace ('-', '_')
        return misc.most_significant_in_dict (sources.__dict__, name, '__')

def get_cli_parser ():
    import optparse
    p = optparse.OptionParser ()

    p.usage = '''settings.py [OPTION]...

Print settings and directory layout.

'''
    p.description = 'Grand Unified Builder.  Settings and directory layout.'

    # c&p #10?
    #import gub_options
    #gub_options.add_common_options (platform,branch,verbose)?
    p.add_option ('-p', '--platform', action='store',
                  dest='platform',
                  type='choice',
                  default=None,
                  help='select target platform',
                  choices=platforms.keys ())
    p.add_option ('-B', '--branch', action='append',
                  dest='branches',
                  default=[],
                  metavar='NAME=BRANCH',
                  help='select branch')
    p.add_option ('-v', '--verbose', action='count', dest='verbose', default=0)
    return p

def as_variables (settings):
    lst = []
    for k in settings.__dict__.keys ():
        v = settings.__dict__[k]
        if type (v) == type (str ()):
            lst.append ('%(k)s=%(v)s' % locals ())
    return lst

def main ():
    cli_parser = get_cli_parser ()
    (options, files) = cli_parser.parse_args ()
    if not options.platform or files:
        raise 'barf'
        sys.exit (2)
    settings = Settings (options)
    print '\n'.join (as_variables (settings))

if __name__ == '__main__':
    main ()