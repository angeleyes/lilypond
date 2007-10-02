class GupPackage:
    "How to package part of an install_root."
    
    def __init__ (self, os_interface):
        self._dict = {}
        self._os_interface = os_interface
        self._file_specs = []
        self._dependencies = []
        self._conflicts = []
        
    # FIXME: move _dict ---> .__dict__
    def set_dict (self, dict, sub_name):
        self._dict = dict.copy ()
        self._dict['sub_name'] = sub_name
        if sub_name:
            sub_name = '-' + sub_name
        s = ('%(name)s' % dict) + sub_name
        self._dict['split_name'] = s
        self._dict['split_ball'] = ('%(packages)s/%(split_name)s%(ball_suffix)s.%(platform)s.gup') % self._dict
        self._dict['split_hdr'] = ('%(packages)s/%(split_name)s%(vc_branch_suffix)s.%(platform)s.hdr') % self._dict
        self._dict['conflicts_string'] = ';'.join (self._conflicts)
        self._dict['dependencies_string'] = ';'.join (self._dependencies)
        self._dict['source_name'] = self.name ()
        if sub_name:
            self._dict['source_name'] = self.name ()[:-len (sub_name)]
        
    def expand (self, s):
        return s % self._dict
    
    def dump_header_file (self):
        import pickle
        hdr = self.expand ('%(split_hdr)s')
        self._os_interface.dump (pickle.dumps (self._dict), hdr)
        
    def clean (self):
        base = self.expand ('%(install_root)s')
        for f in self._file_specs:
            if f and f != '/' and f != '.':
                self._os_interface.system ('rm -rf %(base)s%(f)s ' % locals ())

    def create_tarball (self):
        import os
        from gub import oslog
        path = os.path.normpath (self.expand ('%(install_root)s'))
        suffix = self.expand ('%(packaging_suffix_dir)s')
        split_ball = self.expand ('%(split_ball)s')
        self._os_interface._execute (oslog.PackageGlobs (path,
                                                         suffix,
                                                         self._file_specs,
                                                         split_ball))
    def dict (self):
        return self._dict

    def name (self):
        return '%(split_name)s' % self._dict