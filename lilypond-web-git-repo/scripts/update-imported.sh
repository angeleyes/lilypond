#!/bin/bash

### FIXME: figure out how to get the lastest such dir, not a
###        specific commit.  Or get individual files
### TODO: change url if/when it's merged with master
if test -e snapshot.tar.gz; then
	echo "TEST NICETY: Skipping snapshot download"
else
	wget -O snapshot.tar.gz "http://git.savannah.gnu.org/gitweb/?p=lilypond.git;a=snapshot;h=6625e6514c82cd283100c055b8f2ffa9765efbc6;sf=tgz"
fi

### unpack
tar -xzf snapshot.tar.gz

### simulate merged stuff  (these files won't be master)
rm lilypond/examples/*.png
rm lilypond/SConstruct
rm -rf lilypond/site_scons
rm lilypond/news-to-texi.py

### overwrite imported
rm -rf imported
mv lilypond/ imported/

### clean up
#rm snapshot.tar.gz


