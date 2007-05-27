# -*-Makefile-*-

PACKAGE = git
ALL_PLATFORMS=mingw

MINGIT_BRANCH_FILEIFIED=$(subst /,--,$(MINGIT_BRANCH))
MINGIT_LOCAL_BRANCH=$(MINGIT_BRANCH_FILEIFIED)-repo.or.cz-git-mingw.git

default: all

include gub.make
include compilers.make

PLATFORMS = mingw

GPKG_OPTIONS=--branch git=$(MINGIT_LOCAL_BRANCH)

GUB_OPTIONS=\
 -v -v -v \
 --branch git=$(MINGIT_BRANCH):$(MINGIT_LOCAL_BRANCH)

INSTALLER_BUILDER_OPTIONS=\
  -v -v -v \
  --branch git=$(MINGIT_LOCAL_BRANCH)\
  --version-db uploads/git.versions \

all: $(PLATFORMS)

download:
	$(foreach p, $(PLATFORMS), $(call INVOKE_GUB,$(p)) --online --stage=download  git && ) true
	$(MAKE) update-versions

bootstrap: bootstrap-git download-local local cross-compilers local-cross-tools download 

download-local:
	$(GUB) $(LOCAL_GUB_OPTIONS) -p local\
		--stage=download \
		git pkg-config nsis icoutils 

local:
	cd librestrict && make -f GNUmakefile
	$(GUB) $(LOCAL_GUB_OPTIONS) -p local git 


mingw:
	$(call BUILD,$@,git)

update-versions:
	python gub/versiondb.py --no-sources --url http://lilypond.org/git --dbfile uploads/git.versions --download --platforms="$(PLATFORMS)"

upload:
	scp `ls -1 -t uploads/git*.exe|head -1` hanwen@lilypond.org:www/git/binaries/mingw/