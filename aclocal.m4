dnl WARNING WARNING WARNING WARNING
dnl do not edit! this is aclocal.m4, generated from stepmake/aclocal.m4
dnl aclocal.m4   -*-shell-script-*-
dnl StepMake subroutines for configure.in

AC_DEFUN(AC_STEPMAKE_COMPILE, [
    # -O is necessary to get inlining
    OPTIMIZE=""
    CXXFLAGS=${CXXFLAGS:-""}
    CFLAGS=${CFLAGS:-""}
    checking_b=yes
    optimise_b=no
    profile_b=no
    debug_b=yes

    AC_ARG_ENABLE(checking,
    [  enable-checking         set runtime checks (assert calls). Default: on],
    [checking_b=$enableval] )

    # actually, the default is: tja='-O' to get inlining...
    # off=''  --jcn

    #actually, that sucks.
    #  tja looks like a typo.  Default is optimisation off. --hwn
    
    AC_ARG_ENABLE(optimise,
    [  enable-optimise         use maximal speed optimisations. Default: off],
    [optimise_b=$enableval])
    
    AC_ARG_ENABLE(profiling, 
    [  enable-profiling        compile with gprof support. Default: off],
    [profile_b=$enableval])
    
    AC_ARG_ENABLE(debugging,
    [  enable-debugging        set debug info. Default: on],
    [debug_b=$enableval])

    AC_ARG_ENABLE(mingw-prefix,
    [  enable-mingw-prefix=DIR set the mingw32 directory (standalone windows32 exes)],
    [MINGWPREFIX=$enableval],
    [MINGWPREFIX=no])

    if test "$printing_b" = no; then
	# ugh
	AC_DEFINE(NPRINT)
	DEFINES="$DEFINES -DNPRINT"
    fi
	
    if test "$checking_b" = no; then
	# ugh
	AC_DEFINE(NDEBUG)
	DEFINES="$DEFINES -DNDEBUG"
    fi

    if test "$optimise_b" = yes; then
	OPTIMIZE="-O2 -finline-functions"
    fi

    if test "$optimise_b" = no; then
	OPTIMIZE=""
    fi

    if test $profile_b = yes; then
	EXTRA_LIBES="-pg"
	OPTIMIZE="$OPTIMIZE -pg"
    fi

    if test $debug_b = yes; then	
	OPTIMIZE="$OPTIMIZE -g"
    fi

    # however, C++ support in mingw32 v 0.1.4 is still flaky
    if test x$MINGWPREFIX != xno; then 
	ICFLAGS="-I$MINGWPREFIX/include"
	ILDFLAGS="-$MINGWPREFIX/lib"
    fi

    AC_PROG_CC
    LD='$(CC)'
    AC_SUBST(LD)

    CPPFLAGS=${CPPFLAGS:-""}
    AC_SUBST(CPPFLAGS)
    AC_SUBST(LDFLAGS)
    AC_SUBST(ICFLAGS)
    AC_SUBST(ILDFLAGS)
    AC_SUBST(DEFINES)
    AC_SUBST(EXTRA_LIBES)
])

AC_DEFUN(AC_STEPMAKE_CXX, [
    AC_LANG_CPLUSPLUS
    AC_PROG_CXX

    AC_CHECK_HEADER(FlexLexer.h, true,
	AC_STEPMAKE_WARN(can"\'"t find flex header. Please install Flex headers correctly))

    CPPFLAGS="$CPPFLAGS $DEFINES"
    CXXFLAGS="$CXXFLAGS $OPTIMIZE"
    LDFLAGS=$EXTRA_LIBES

    AC_SUBST(CXXFLAGS)
    AC_SUBST(CXX)
    LD='$(CXX)'
    AC_SUBST(LD)
])

AC_DEFUN(AC_STEPMAKE_CXXTEMPLATE, [
    AC_CACHE_CHECK([whether explicit instantiation is needed],
	lily_cv_need_explicit_instantiation,
	AC_TRY_LINK([
    template <class T> struct foo { static int baz; };
    template <class T> int foo<T>::baz = 1;
    ], [ return foo<int>::baz; ],
	    lily_cv_need_explicit_instantiation=no,
	    lily_cv_need_explicit_instantiation=yes))
    if test x"$lily_cv_need_explicit_instantiation"x = x"yes"x; then
	AC_DEFINE(NEED_EXPLICIT_INSTANTIATION)
    fi
])

AC_DEFUN(AC_STEPMAKE_DATADIR, [
    if test "$datadir" = "\${prefix}/share"; then
	    datadir='${prefix}/share/'$package
    fi
    DIR_DATADIR=${datadir}
    presome=${prefix}
    if test "$prefix" = "NONE"; then
	    presome=${ac_default_prefix}
    fi
    DIR_DATADIR=`echo ${DIR_DATADIR} | sed "s!\\\${prefix}!$presome!"`
    AC_SUBST(datadir)
    AC_SUBST(DIR_DATADIR)
    AC_DEFINE_UNQUOTED(DIR_DATADIR, "${DIR_DATADIR}")
])

AC_DEFUN(AC_STEPMAKE_END, [
    AC_OUTPUT($CONFIGFILE.make:config.make.in)

    rm -f GNUmakefile
    cp make/toplevel.make.in ./GNUmakefile
    chmod 444 GNUmakefile
])

AC_DEFUN(AC_STEPMAKE_GXX, [
    # ugh autoconf
    changequote(<<, >>)dnl
    if $CXX --version | grep '2\.[78]' > /dev/null ||
	$CXX --version | grep 'egcs' > /dev/null
    changequote([, ])dnl
    then
	    true
    else
	    AC_STEPMAKE_WARN(can\'t find g++ 2.7, 2.8 or egcs)
    fi
])

AC_DEFUN(AC_STEPMAKE_GUILE, [
    # on some systems, -lguile succeeds for guile-1.3
    # others need readline, dl (or even more)
    # urg, must check for different functions in libguile
    # to force new check iso reading from cache
    AC_CHECK_LIB(guile, scm_shell, \
      LIBS="-lguile $LIBS"; AC_DEFINE(HAVE_LIBGUILE), \
      AC_CHECK_LIB(readline, readline) \
      AC_CHECK_LIB(dl, dlopen) \
      AC_CHECK_LIB(socket, socket)\
      AC_CHECK_LIB(termcap,tgetent)\
      AC_CHECK_LIB(m, fabs)\
      AC_CHECK_LIB(guile, scm_boot_guile)\
    )
    if test "$ac_cv_lib_guile_scm_shell" != yes -a \
      "$ac_cv_lib_scm_boot_guile" != yes ; then
	    AC_STEPMAKE_WARN(You should install guile 1.3 or newer)
    fi
])

AC_DEFUN(AC_STEPMAKE_INIT, [

    . $srcdir/VERSION
    FULL_VERSION=$MAJOR_VERSION.$MINOR_VERSION.$PATCH_LEVEL
    if test x$MY_PATCH_LEVEL != x; then
	FULL_VERSION=$FULL_VERSION.$MY_PATCH_LEVEL
    fi

    # urg: don't "fix" this: irix doesn't know about [:lower:] and [:upper:]
    changequote(<<, >>)dnl
    PACKAGE=`echo $PACKAGE_NAME | tr '[a-z]' '[A-Z]'`
    package=`echo $PACKAGE_NAME | tr '[A-Z]' '[a-z]'`
    changequote([, ])dnl

    # No versioning on directory names of sub-packages 
    # urg, urg
    stepmake=${datadir}/stepmake
    presome=${prefix}
    if test "$prefix" = "NONE"; then
	    presome=${ac_default_prefix}
    fi
    stepmake=`echo ${stepmake} | sed "s!\\\${prefix}!$presome!"`

    if test "x$PACKAGE" = "xSTEPMAKE"; then
        echo Stepmake package!
	(cd stepmake; rm -f stepmake; ln -s ../stepmake .)
	(cd stepmake; rm -f bin; ln -s ../bin .)
	AC_CONFIG_AUX_DIR(bin)
	stepmake=stepmake
    else
        echo Package: $PACKAGE
	# Check for installed stepmake
	if test -d $stepmake; then
	    echo Using installed stepmake: $stepmake
	else
	    stepmake='$(depth)'/stepmake
	    echo Using local stepmake: $datadir/stepmake not found
	fi
	AC_CONFIG_AUX_DIR(\
	  $HOME/usr/local/share/stepmake/bin\
	  $HOME/usr/local/lib/stepmake/bin\
	  $HOME/usr/share/stepmake/bin\
	  $HOME/usr/lib/stepmake/bin\
	  /usr/local/share/stepmake/bin\
	  /usr/local/lib/stepmake/bin\
	  /usr/share/stepmake/bin\
	  /usr/lib/stepmake/bin\
	  stepmake/bin\
	)
    fi

    AC_SUBST(stepmake)
    AC_SUBST(package)
    AC_SUBST(PACKAGE)
    AC_SUBST(PACKAGE_NAME)
    AC_DEFINE_UNQUOTED(PACKAGE, "${PACKAGE_NAME}")
    AC_DEFINE_UNQUOTED(TOPLEVEL_VERSION, "${FULL_VERSION}")

    package_depth=`dirname $cache_file`
    AC_SUBST(package_depth)

    AUTOGENERATE="This file was automatically generated by configure"
    AC_SUBST(AUTOGENERATE)
    absolute_builddir="`pwd`"
    AC_SUBST(absolute_builddir)

    STATE_VECTOR=`ls make/STATE-VECTOR 2>/dev/null`
    if test "x$STATE_VECTOR" != "x"; then
    	STATE_VECTOR="\$(depth)/$STATE_VECTOR"
    fi
    AC_SUBST(STATE_VECTOR)

    CONFIGSUFFIX=
    AC_ARG_ENABLE(config,
    [  enable-config=FILE      put configure settings in config-FILE.make],
    [CONFIGSUFFIX=$enableval])

    if test "$CONFIGSUFFIX" != "" ; then
	CONFIGFILE=config-$CONFIGSUFFIX
    else
	CONFIGFILE=config
    fi
    AC_SUBST(CONFIGSUFFIX)
     
    AC_CANONICAL_HOST
    AC_CHECK_PROGS(MAKE, make, error)
    AC_CHECK_PROGS(FIND, find, error)


dnl system supplied INSTALL is unsafe; use our own install.
dnl    AC_PROG_INSTALL
dnl    if test "$INSTALL" = "bin/install-sh"; then
dnl	export INSTALL="\$\(depth\)/bin/install-sh"
dnl    fi

    AC_CHECK_PROGS(TAR, tar, error)
    AC_CHECK_PROGS(BASH, bash, /bin/sh)

    AC_PATH_PROG(PYTHON, ${PYTHON:-python}, -echo no python)
    AC_SUBST(PYTHON)


    AC_CHECK_SEARCH_RESULT($MAKE, GNU make,  You should install GNU make)
    if test $MAKE != "error" ; then
	$MAKE -v| grep GNU > /dev/null
	if test "$?" = 1
	then
		AC_STEPMAKE_WARN(Please install *GNU* make) 
	fi 
    fi 

    AC_CHECK_SEARCH_RESULT($PYTHON, python, You should install Python)

    if test "x$OSTYPE" = "xcygwin32" || test "x$OSTYPE" = "xWindows_NT"; then
	LN=cp # hard link does not work under cygnus-nt
	LN_S=cp # symbolic link does not work for native nt
	ZIP="zip -r -9" #
	DOTEXE=.exe
       DIRSEP='\\'
 	PATHSEP=';'
       INSTALL="\$(SHELL) \$(stepdir)/../bin/install-dot-exe.sh -c"
    else
	DIRSEP='/'
	PATHSEP=':'
	LN=ln
	LN_S='ln -s'
	ZIP="zip -r -9"
        INSTALL="\$(SHELL) \$(stepdir)/../bin/install-sh -c"
    fi
    AC_SUBST(DOTEXE)
    AC_SUBST(ZIP)
    AC_SUBST(LN)
    AC_SUBST(LN_S)
    AC_SUBST(INSTALL)
    AC_DEFINE_UNQUOTED(DIRSEP, '${DIRSEP}')
    AC_DEFINE_UNQUOTED(PATHSEP, '${PATHSEP}')
    AC_SUBST(PATHSEP)
    AC_SUBST(DIRSEP)
  
   
    AC_STEPMAKE_DATADIR
])

AC_DEFUN(AC_STEPMAKE_LEXYACC, [
    # ugh, automake: we want (and check for) bison
    AC_PROG_YACC
    # ugh, automake: we want (and check for) flex
    # AC_PROG_LEX
    # urg: automake 1.3: hope this doesn't break 1.2 ac_cv_pro_lex_root hack...

    # AC_DECL_YYTEXT
    # ugh, ugh
    ac_cv_prog_lex_root=lex.yy

    AC_CHECK_PROGS(BISON, bison, error)
    AC_CHECK_PROGS(FLEX, flex, error)
    AC_CHECK_SEARCH_RESULT($BISON, bison,  Please install Bison, 1.25 or newer)
    AC_CHECK_SEARCH_RESULT($FLEX,  flex, Please install Flex, 2.5 or newer)

    if test $BISON != "error"; then
	bison_version=`$BISON --version| sed 's/^.*version 1.//g' `
	if test $bison_version -lt 25; then
	    AC_STEPMAKE_WARN(Your bison is a bit old (1.$bison_version). You might have to install 1.25)
	fi	
    fi

    AC_SUBST(BISON)
    AC_SUBST(FLEX)
])

AC_DEFUN(AC_STEPMAKE_LIB, [
    AC_CHECK_PROGS(AR, ar, error)
    AC_PROG_RANLIB

    AC_SUBST(AR)
    AC_SUBST(RANLIB)
])

AC_DEFUN(AC_STEPMAKE_LIBTOOL, [
    # libtool.info ...
    # **Never** try to set library version numbers so that they correspond
    # to the release number of your package.  This is an abuse that only
    # fosters misunderstanding of the purpose of library versions.

    REVISION=$PATCH_LEVEL
    # CURRENT=$MINOR_VERSION
    CURRENT=`expr $MINOR_VERSION + 1`
    # AGE=$(expr $MAJOR_VERSION + 1)
    AGE=$MAJOR_VERSION
    AC_SUBST(CURRENT)
    AC_SUBST(REVISION)
    AC_SUBST(AGE)
])

AC_DEFUN(AC_STEPMAKE_LOCALE, [
    lang=English
    ALL_LINGUAS="en nl"

    # with/enable ??
    AC_ARG_WITH(localedir,
    [  with-localedir=LOCALE   use LOCALE as locale dir. Default: PREFIX/share/locale ],
    localedir=$with_localedir,
    localedir='${prefix}/share/locale')

    AC_ARG_WITH(lang,
    [  with-lang=LANG          use LANG as language to emit messages],
    language=$with_lang,
    language=English)

    AC_MSG_CHECKING(language)    
    case "$language" in
      En* | en* | Am* | am* | US* | us*)
	    lang=English;;
      NL | nl | Du* | du* | Ned* | ned*)
	    lang=Dutch;;
      "")
	    lang=English;;
      *)
	    lang=unknown;;
    esac
    AC_MSG_RESULT($lang)

    if test "$lang" = "unknown" ; then
	AC_STEPMAKE_WARN($language not supported; available are: $ALL_LINGUAS)
    fi

])

AC_DEFUN(AC_STEPMAKE_GETTEXT, [
    DIR_LOCALEDIR=${localedir}
    presome=${prefix}
    if test "$prefix" = "NONE"; then
	    presome=${ac_default_prefix}
    fi
    DIR_LOCALEDIR=`echo ${DIR_LOCALEDIR} | sed "s!\\\${prefix}!$presome!"`
    AC_SUBST(localedir)
    AC_SUBST(DIR_LOCALEDIR)
    AC_DEFINE_UNQUOTED(DIR_LOCALEDIR, "${DIR_LOCALEDIR}")

    AC_CHECK_LIB(intl, gettext)
    AC_CHECK_FUNCS(gettext)
])

AC_DEFUN(AC_STEPMAKE_MAN, [
    AC_CHECK_PROGS(TROFF, troff, -echo no troff)
])

AC_DEFUN(AC_STEPMAKE_MSGFMT, [
    # AC_CHECK_PROGS(MSGFMT, msgfmt, -echo no msgfmt)
    AC_CHECK_PROGS(MSGFMT, msgfmt, \$(SHELL) \$(step-bindir)/fake-msgfmt.sh )
    AC_MSG_CHECKING(whether msgfmt accepts -o)
    msgfmt_output="`msgfmt -o bla 2>&1 | grep usage`"
    if test "$msgfmt_output" = ""; then
	AC_MSG_RESULT(yes)
    else
	# urg
	MSGFMT="\$(SHELL) \$(step-bindir)/fake-msgfmt.sh"
	AC_MSG_RESULT(no)
	AC_STEPMAKE_WARN(please install msgfmt from GNU gettext)
    fi
    if test ! -n "$MSGFMT"; then
	AC_STEPMAKE_WARN(please install msgfmt from GNU gettext)
    fi
])

AC_DEFUN(AC_STEPMAKE_TEXMF_DIRS, [
    AC_ARG_ENABLE(tex-prefix,
    [  enable-tex-prefix=DIR   set the tex-directory to find TeX subdirectories. (default: PREFIX)],
    [TEXPREFIX=$enableval],
    [TEXPREFIX=auto] )
    
    AC_ARG_ENABLE(tex-dir,
    [  enable-tex-dir=DIR      set the directory to put $PACKAGE_NAME TeX files in. ],
    [TEXDIR=$enableval],
    [TEXDIR=auto] )

    AC_ARG_ENABLE(mf-dir,
    [  enable-mf-dir=DIR       set the directory to put $PACKAGE_NAME MetaFont files in. ],
    [MFDIR=$enableval],
    [MFDIR=auto])

    if test "x$TEXPREFIX" = xauto ; then
	AC_TEX_PREFIX(TEXPREFIX)
    else
     find_texprefix=$TEXPREFIX
    fi

    if test "x$MFDIR" = xauto; then
	AC_MF_SUBDIR(MFDIR)
    fi
	
    if test "x$TEXDIR" = xauto ; then
	AC_TEX_SUBDIR(TEXDIR)
    fi
    AC_SUBST(TEXPREFIX)
    AC_SUBST(TEXDIR)
    AC_SUBST(MFDIR)
])

AC_DEFUN(AC_STEPMAKE_TEXMF, [
    # urg, never know what names these teTeX guys will think up
    AC_CHECK_PROGS(METAFONT, mf, no)
    if test "x$METAFONT" = "xno"; then
	AC_CHECK_PROGS(MFONT, mfont, -echo no mf or mfont)
	METAFONT=$MFONT
    fi

    AC_CHECK_PROGS(METAPOST, mp, no)
    if test "x$METAPOST" = "xno"; then
	AC_CHECK_PROGS(MPOST, mpost, -echo no mp or mpost)
	METAPOST=$MPOST
    fi

    AC_SUBST(METAFONT)
    AC_SUBST(METAPOST)
])

AC_DEFUN(AC_STEPMAKE_WARN, [
    AC_MSG_WARN($1)
    warn_b=yes
])

AC_DEFUN(AC_STEPMAKE_YODL, [
    if test "x$YODL" = "x"; then 
	AC_CHECK_PROGS(STRIPROFF, striproff, -echo no striproff)
	AC_CHECK_PROGS(YODL, yodl, -echo no yodl)
	AC_CHECK_PROGS(YODL2HTML, yodl2html, -echo no yodl)
	AC_CHECK_PROGS(YODL2LATEX, yodl2latex, )
	AC_CHECK_PROGS(YODL2MAN, yodl2man, -echo no yodl)
	AC_CHECK_PROGS(YODL2MSLESS, yodl2msless, -echo no yodl)
	AC_CHECK_PROGS(YODL2TEXINFO, yodl2texinfo, -echo no yodl)
	AC_CHECK_PROGS(YODL2TXT, yodl2txt, -echo no yodl)
	YODL2LESS_DIR='$(bindir)/'
    else
	AC_SUBST(STRIPROFF)
	AC_SUBST(YODL)
	AC_SUBST(YODL2HTML)
	AC_SUBST(YODL2LATEX)
	AC_SUBST(YODL2LESS_DIR)
	AC_SUBST(YODL2MAN)
	AC_SUBST(YODL2MSLESS)
	AC_SUBST(YODL2TEXINFO)
	AC_SUBST(YODL2TXT)
	export STRIPROFF YODL YODL2HTML YODL2LATEX YODL2MAN YODL2MSLESS YODL2TEXINFO YODL2TXT
    fi
    if test "x$YODL" = "-echo no yodl"; then
	AC_STEPMAKE_WARN(Did not find YODL (Yodl is Yet Oneother Document Language, see http://www.cs.uu.nl/~hanwen/yodl))
    fi    
])

dnl should cache result.
dnl should  look in $prefix first.
dnl should probably assume TDS

AC_DEFUN(AC_TEX_PREFIX, [
    

    AC_MSG_CHECKING(TeX/MF root dir directory)    

    find_root_prefix="$prefix"
    

    test "x$find_root_prefix" = xNONE && find_root_prefix="$ac_default_prefix"
    find_texpostfix="";
    for postfix in "/lib/tex/" "/lib/texmf" "/lib" "/tex" "/texmf"; do
	find_texprefix="$find_root_prefix$postfix"
	if test -d $find_texprefix; then
	    find_texpostfix=$postfix
	    break;
	fi
    done
    
    if test "x$find_texpostfix" = x; then
	find_texpostfix='/lib/texmf/tex'
	AC_STEPMAKE_WARN(Cannot determine the TeX-directory. Please use --enable-tex-prefix)
    fi

    find_texprefix="$find_root_prefix/$find_texpostfix"

    # only assign if variablename not empty
    if test x != "x[$]$1"; then
    	$1='${prefix}'/"$find_texpostfix"
    fi
    AC_MSG_RESULT($find_texprefix)

])
 

# find a directory inside a prefix, 
# $1 the prefix (expanded version)
# $2 variable to assign
# $3 the directory name 
# $4 description
AC_DEFUN(AC_FIND_DIR_IN_PREFIX, [
    
    AC_MSG_CHECKING($4 directory)    
    find_dirdir=`(cd $1; 
      $FIND ./ -type d -a -name $3 -print |sort|head -1|sed 's#^\./##')`
    

    if test "x$find_dirdir" = x; then
       find_dirdir="/$3";
       AC_STEPMAKE_WARN(Cannot determine $4 subdirectory. Please set from command-line)
	true
    fi
    $2=$find_dirdir
    AC_MSG_RESULT($1/$find_dirdir)
])

# ugh.  this is hopeless
AC_DEFUN(AC_KPSE_TEX_DIR, [
	kpse_paths=`(kpsepath -n latex tex; kpsepath -n tex tex) | sed 's/:/ /g' | tr ' ' '\012' |sort | uniq -d`
	kpse_syspaths=`echo $kpse_paths | grep '!'| sed 's/!//g'`
	echo $kpse_paths
	if test -w "$kpse_syspaths";
	then
		dir=`echo $kpse_syspaths | head -1`
	else
		dir=`echo $kpse_paths | grep -v '!'| head -1`
	fi
	if test "$prefix" = "NONE"; then
		local_prefix=$ac_default_prefix
		local_prefix_quote='${prefix}'

	else
		local_prefix=$prefix
		local_prefix_quote=$prefix
	fi
	echo $local_prefix_quote = $local_prefix
	echo $dir
	echo $dir  | sed 's!'$local_prefix'!\$local_prefix_quote!g'
])

AC_DEFUN(AC_TEX_SUBDIR, [
dnl    AC_REQUIRE([AC_TEX_PREFIX])
    AC_FIND_DIR_IN_PREFIX($find_texprefix, $1, tex,TeX input)
    $1="$TEXPREFIX/$$1"
])

AC_DEFUN(AC_MF_SUBDIR, [
dnl     AC_REQUIRE([AC_TEX_PREFIX])
    AC_FIND_DIR_IN_PREFIX($find_texprefix, $1, source, MF input)
    $1="$TEXPREFIX/$$1"
])

AC_DEFUN(AC_CHECK_SEARCH_RESULT, [
	result="`echo \"$1\" | grep echo`"
	if test "x$1" = "xerror" -o "x$result" != "x"; then
		AC_STEPMAKE_WARN(can\'t find $2. $3)
	fi
])


# Configure paths for GTK+
# Owen Taylor     97-11-3

dnl AM_PATH_GTK([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for GTK, and define GTK_CFLAGS and GTK_LIBS
dnl
AC_DEFUN(AM_PATH_GTK,
[dnl 
dnl Get the cflags and libraries from the gtk-config script
dnl
  AC_PATH_PROG(GTK_CONFIG, gtk-config, no)
  min_gtk_version=ifelse([$1], ,1.1.1,$1)
  AC_MSG_CHECKING(for GTK - version >= $min_gtk_version)
  no_gtk=""
  if test "$GTK_CONFIG" != "no" ; then
    GTK_CFLAGS=`$GTK_CONFIG --cflags`
    GTK_LIBS=`$GTK_CONFIG --libs`
    ac_save_CFLAGS="$CFLAGS"
    ac_save_LIBS="$LIBS"
    ac_save_CXXFLAGS="$CXXFLAGS"
    CFLAGS="$CFLAGS $GTK_CFLAGS"
    CXXFLAGS="$CXXFLAGS $GTK_CFLAGS"
    LIBS="$LIBS $GTK_LIBS"
dnl
dnl Now check if the installed GTK is sufficiently new. (Also sanity
dnl checks the results of gtk-config to some extent
dnl
    AC_TRY_RUN([
#include <gtk/gtk.h>
#include <stdio.h>

int 
main ()
{
  int major, minor, micro;

  if (sscanf("$min_gtk_version", "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_gtk_version");
     exit(1);
   }

   return !((gtk_major_version > major) ||
   	    ((gtk_major_version == major) && (gtk_minor_version > minor)) ||
 	    ((gtk_major_version == major) && (gtk_minor_version == minor) && (gtk_micro_version >= micro)));
}
],, no_gtk=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
     CFLAGS="$ac_save_CFLAGS"
     CXXFLAGS="$ac_save_CXXFLAGS"
     LIBS="$ac_save_LIBS"
  else
     no_gtk=yes
  fi
  if test "x$no_gtk" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     GTK_CFLAGS=""
     GTK_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  CXXFLAGS="$CXXFLAGS $GTK_CFLAGS"
  AC_SUBST(CXXFLAGS)
  AC_SUBST(GTK_CFLAGS)
  AC_SUBST(GTK_LIBS)
])


# Configure paths for GTK--
# Erik Andersen	30 May 1998
# Modified by Tero Pulkkinen (added the compiler checks... I hope they work..)

dnl Test for GTK__, and define GTK___CFLAGS and GTK___LIBS
dnl   to be used as follows:
dnl AM_PATH_GTKMM([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl

dnl Get the cflags and libraries from the gtkmm-config script
dnl
AC_ARG_WITH(gtkmm-prefix,[  --with-gtkmm-prefix=PREFIX
                          Prefix where GTK-- is installed (optional)],
            gtkmm_config_prefix="$withval", gtkmm_config_prefix="")
AC_ARG_WITH(gtkmm-exec-prefix,[  --with-gtkmm-exec-prefix=PREFIX
                          Exec prefix where GTK-- is installed (optional)],
            gtkmm_config_exec_prefix="$withval", gtkmm_config_exec_prefix="")
AC_ARG_ENABLE(gtkmmtest, [  --disable-gtkmmtest     Do not try to compile and run a test GTK-- program],
		    , enable_gtkmmtest=yes)

  if test x$gtkmm_config_exec_prefix != x ; then
     gtkmm_config_args="$gtkmm_config_args --exec-prefix=$gtkmm_config_exec_prefix"
     if test x${GTKMM_CONFIG+set} != xset ; then
        GTKMM_CONFIG=$gtkmm_config_exec_prefix/bin/gtkmm-config
     fi
  fi
  if test x$gtkmm_config_prefix != x ; then
     gtkmm_config_args="$gtkmm_config_args --prefix=$gtkmm_config_prefix"
     if test x${GTKMM_CONFIG+set} != xset ; then
        GTKMM_CONFIG=$gtkmm_config_prefix/bin/gtkmm-config
     fi
  fi


AC_DEFUN(AM_PATH_GTKMM,
[dnl 

dnl
dnl Check check if the installed GTK-- is sufficiently new.
dnl
  AC_PATH_PROG(GTKMM_CONFIG, gtkmm-config, no)
  min_gtkmm_version=ifelse([$1], ,0.9.14,$1)

  AC_MSG_CHECKING(for GTK-- - version >= $min_gtkmm_version)
  no_gtkmm=""
  if test "$GTKMM_CONFIG" = "no" ; then
    no_gtkmm=yes
  else
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS

    GTK___CFLAGS=`$GTKMM_CONFIG $gtkmm_config_args --cflags`
    GTK___LIBS=`$GTKMM_CONFIG $gtkmm_config_args --libs`
    gtkmm_config_major_version=`$GTKMM_CONFIG $gtkmm_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    gtkmm_config_minor_version=`$GTKMM_CONFIG $gtkmm_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    gtkmm_config_micro_version=`$GTKMM_CONFIG $gtkmm_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_gtkmmtest" = "xyes" ; then
      ac_save_CXXFLAGS="$CXXFLAGS"
      ac_save_LIBS="$LIBS"
      CXXFLAGS="$CXXFLAGS $GTK___CFLAGS"
      LIBS="$LIBS $GTK___LIBS"
dnl
dnl Now check if the installed GTK-- is sufficiently new. (Also sanity
dnl checks the results of gtkmm-config to some extent
dnl
      rm -f conf.gtkmmtest
      AC_TRY_RUN([
#include <gtk--.h>
#include <stdio.h>
#include <stdlib.h>

int 
main ()
{
  int major, minor, micro;
  char *tmp_version;

  system ("touch conf.gtkmmtest");

  /* HP/UX 0 (%@#!) writes to sscanf strings */
  tmp_version = g_strdup("$min_gtkmm_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_gtkmm_version");
     exit(1);
   }

  if ((gtkmm_major_version != $gtkmm_config_major_version) ||
      (gtkmm_minor_version != $gtkmm_config_minor_version) ||
      (gtkmm_micro_version != $gtkmm_config_micro_version))
    {
      printf("\n*** 'gtkmm-config --version' returned %d.%d.%d, but GTK-- (%d.%d.%d)\n", 
             $gtkmm_config_major_version, $gtkmm_config_minor_version, $gtkmm_config_micro_version,
             gtkmm_major_version, gtkmm_minor_version, gtkmm_micro_version);
      printf ("*** was found! If gtkmm-config was correct, then it is best\n");
      printf ("*** to remove the old version of GTK--. You may also be able to fix the error\n");
      printf("*** by modifying your LD_LIBRARY_PATH enviroment variable, or by editing\n");
      printf("*** /etc/ld.so.conf. Make sure you have run ldconfig if that is\n");
      printf("*** required on your system.\n");
      printf("*** If gtkmm-config was wrong, set the environment variable GTKMM_CONFIG\n");
      printf("*** to point to the correct copy of gtkmm-config, and remove the file config.cache\n");
      printf("*** before re-running configure\n");
    } 
/* GTK-- does not have the GTKMM_*_VERSION constants */
/* 
  else if ((gtkmm_major_version != GTKMM_MAJOR_VERSION) ||
	   (gtkmm_minor_version != GTKMM_MINOR_VERSION) ||
           (gtkmm_micro_version != GTKMM_MICRO_VERSION))
    {
      printf("*** GTK-- header files (version %d.%d.%d) do not match\n",
	     GTKMM_MAJOR_VERSION, GTKMM_MINOR_VERSION, GTKMM_MICRO_VERSION);
      printf("*** library (version %d.%d.%d)\n",
	     gtkmm_major_version, gtkmm_minor_version, gtkmm_micro_version);
    }
*/
  else
    {
      if ((gtkmm_major_version > major) ||
        ((gtkmm_major_version == major) && (gtkmm_minor_version > minor)) ||
        ((gtkmm_major_version == major) && (gtkmm_minor_version == minor) && (gtkmm_micro_version >= micro)))
      {
        return 0;
       }
     else
      {
        printf("\n*** An old version of GTK-- (%d.%d.%d) was found.\n",
               gtkmm_major_version, gtkmm_minor_version, gtkmm_micro_version);
        printf("*** You need a version of GTK-- newer than %d.%d.%d. The latest version of\n",
	       major, minor, micro);
        printf("*** GTK-- is always available from ftp://ftp.gtk.org.\n");
        printf("***\n");
        printf("*** If you have already installed a sufficiently new version, this error\n");
        printf("*** probably means that the wrong copy of the gtkmm-config shell script is\n");
        printf("*** being found. The easiest way to fix this is to remove the old version\n");
        printf("*** of GTK--, but you can also set the GTKMM_CONFIG environment to point to the\n");
        printf("*** correct copy of gtkmm-config. (In this case, you will have to\n");
        printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
        printf("*** so that the correct libraries are found at run-time))\n");
      }
    }
  return 1;
}
],, no_gtkmm=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CXXFLAGS="$ac_save_CXXFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_gtkmm" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$GTKMM_CONFIG" = "no" ; then
       echo "*** The gtkmm-config script installed by GTK-- could not be found"
       echo "*** If GTK-- was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the GTK_CONFIG environment variable to the"
       echo "*** full path to gtk-config."
       echo "*** The gtkmm-config script was not available in GTK-- versions"
       echo "*** prior to 0.9.12. Perhaps you need to update your installed"
       echo "*** version to 0.9.12 or newer"
     else
       if test -f conf.gtkmmtest ; then
        :
       else
          echo "*** Could not run GTK-- test program, checking why..."
          CXXFLAGS="$CFLAGS $GTKMM_CXXFLAGS"
          LIBS="$LIBS $GTK___LIBS"
          AC_TRY_LINK([
#include <gtk--.h>
#include <stdio.h>
],      [ return ((gtkmm_major_version) || (gtkmm_minor_version) || (gtkmm_micro_version)); ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding GTK-- or finding the wrong"
          echo "*** version of GTK--. If it is not finding GTK--, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means GTK-- was incorrectly installed"
          echo "*** or that you have moved GTK-- since it was installed. In the latter case, you"
          echo "*** may want to edit the gtkmm-config script: $GTKMM_CONFIG" ])
          CXXFLAGS="$ac_save_CXXFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     GTK___CFLAGS=""
     GTK__LIBS=""
     ifelse([$3], , :, [$3])
     AC_LANG_RESTORE
  fi
  AC_SUBST(GTK___CFLAGS)
  AC_SUBST(GTK___LIBS)
  rm -f conf.gtkmmtest
])

# Configure paths for GTK--DRAW
# Derek Quinn Wyatt   98-08-21  (adapted from Jan Nieuwenhuizen's code)

dnl AM_PATH_GTK__DRAW([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for GTK--DRAW, and define GTK___CFLAGS and GTK___LIBS
dnl
AC_DEFUN(AM_PATH_GTK__DRAW,
[dnl 
dnl Get the cflags and libraries from the gtk__-config script
dnl
  AC_PATH_PROG(GTKMM_CONFIG, gtkmm-config, no)
  min_gtk___version=ifelse([$1], ,0.0.5,$1)
  AC_MSG_CHECKING(for GTK--DRAW - version >= $min_gtk___version)
  no_gtk__=""
  if test "$GTKMM_CONFIG" != "no" ; then
    GTK___CFLAGS=`$GTKMM_CONFIG --cflags`
    GTK___LIBS=`$GTKMM_CONFIG --libs`
    GTK___DLIBS="$GTK___LIBS -lgtkmmdraw"
    GTK___LIBS="$GTK___DLIBS"
    ac_save_CFLAGS="$CFLAGS"
    ac_save_LIBS="$LIBS"
    ac_save_CXXFLAGS="$CXXFLAGS"
    CFLAGS="$CFLAGS $GTK___CFLAGS"
    CXXFLAGS="$CXXFLAGS $GTK___CFLAGS"
    LIBS="$LIBS $GTK___LIBS"
dnl
dnl Now check if the installed GTK__ is sufficiently new. (Also sanity
dnl checks the results of gtk__-config to some extent
dnl
    AC_TRY_RUN([
#include <gtk--.h>
#include <stdio.h>

int 
main ()
{
  // urg
  return 0;
}
],, no_gtk__=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
     CFLAGS="$ac_save_CFLAGS"
     CXXFLAGS="$ac_save_CXXFLAGS"
     LIBS="$ac_save_LIBS"
  else
     no_gtk__=yes
  fi
  if test "x$no_gtk__" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     GTK___CFLAGS=""
     GTK___LIBS=""
     ifelse([$3], , :, [$3])
  fi
  CXXFLAGS="$CXXFLAGS $GTK___CFLAGS"
  AC_SUBST(CXXFLAGS)
  AC_SUBST(GTK___CFLAGS)
  AC_SUBST(GTK___LIBS)
])

