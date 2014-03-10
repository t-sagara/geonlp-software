#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([geonlp], [1.0.7], [geonlp@info-proto.com])
AM_INIT_AUTOMAKE([geonlp], [1.0.7])
AC_CONFIG_SRCDIR([src/])
AC_CONFIG_HEADER([include/config.h])
AC_CONFIG_MACRO_DIR([m4])

# library version
LIB_VERSION_INFO='1.0.7'
AC_SUBST(LIB_VERSION_INFO)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET
AM_PROG_LIBTOOL

# Checks for libraries.
AC_HAVE_LIBRARY([boost_system-mt],
  LIBBOOST_SYSTEM_LIB="-lboost_system-mt",
  AC_HAVE_LIBRARY([boost_system],
    LIBBOOST_SYSTEM_LIB="-lboost_system",
    AC_MSG_ERROR(*** BOOST_SYSTEM/BOOST_SYSTEM-MT library not found ***)
  )
)
AC_SUBST(LIBBOOST_SYSTEM_LIB)

AC_HAVE_LIBRARY([boost_filesystem-mt],
  LIBBOOST_FILESYSTEM_LIB="-lboost_filesystem-mt",
  AC_HAVE_LIBRARY([boost_filesystem],
    LIBBOOST_FILESYSTEM_LIB="-lboost_filesystem",
    AC_MSG_ERROR(*** BOOST_FILESYSTEM/BOOST_FILESYSTEM-MT library not found ***)
  )
)
AC_SUBST(LIBBOOST_FILESYSTEM_LIB)

AC_HAVE_LIBRARY([boost_regex-mt],
  LIBBOOST_REGEX_LIB="-lboost_regex-mt",
  AC_HAVE_LIBRARY([boost_regex],
    LIBBOOST_REGEX_LIB="-lboost_regex",
    AC_MSG_ERROR(*** BOOST_REGEX/BOOST_REGEX-MT library not found ***)
  )
)
AC_SUBST(LIBBOOST_REGEX_LIB)

AC_HAVE_LIBRARY([boost_thread-mt],
  LIBBOOST_THREAD_LIB="-lboost_thread-mt",
  AC_HAVE_LIBRARY([boost_thread],
    LIBBOOST_THREAD_LIB="-lboost_thread",
    AC_MSG_ERROR(*** BOOST_THREAD/BOOST_THREAD-MT library not found ***)
  )
)
AC_SUBST(LIBBOOST_THREAD_LIB)

# MeCab installation settings
AC_HAVE_LIBRARY([mecab],
  LIBMECAB_LIB="-lmecab",
  AC_MSG_ERROR(*** MECAB library not found ***)
)
AC_SUBST(LIBMECAB_LIB)

AC_ARG_WITH(
   mecab-config,
   [  --with-mecab-config=PATH  set mecab-config location [search path]],
   AC_MSG_RESULT(using $with_mecab_config for mecab-config)
   MECAB_CONFIG=$with_mecab_config,
   [AC_PATH_PROGS(MECAB_CONFIG, mecab-config, no)]
)

if test $MECAB_CONFIG = "no";
then
   AC_MSG_ERROR(mecab-config is not found in your system)
fi
AC_SUBST(MECAB_CONFIG)

MECAB_DICT_INDEX="`$MECAB_CONFIG --libexecdir`/mecab-dict-index"
AC_SUBST(MECAB_DICT_INDEX)


# DAMS installation settings
AC_ARG_WITH(
   [dams],
   [  --with-dams=PATH  set geocoder dams install path],
   [with_dams=$with_dams],
   [with_dams=yes]
)

LIBDAMS_LIB=""
DAMS_INC_FLAGS=""
AS_IF([test "x$with_dams" != xyes],
  [AS_IF([test "x$with_dams" != xno],
    [AC_DEFINE([HAVE_LIBDAMS], [1], [Define if you  have libdams]),
     LIBDAMS_LIB="-L${with_dams}/lib -ldams"
     DAMS_INC_FLAGS="-I${with_dams}/include"],
    [LIBDAMS_LIB=""]
  )],
  [AC_HAVE_LIBRARY([dams],
    [LIBDAMS_LIB="-ldams"
     AC_DEFINE([HAVE_LIBDAMS], [1], [use dams])],
    [LIBDAMS_LIB=""])
  ]
)
AC_SUBST(LIBDAMS_LIB)
AC_SUBST(DAMS_INC_FLAGS)

# misc libraries
AC_CHECK_LIB([iconv], [iconv_open])

AC_HAVE_LIBRARY([sqlite3],
  LIBSQLITE3_LIB="-lsqlite3",
  AC_MSG_ERROR(*** Sqlite3 library not found ***)
)
AC_SUBST(LIBSQLITE3_LIB)

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h stdlib.h string.h, sqlite3.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_STRTOD
AC_CHECK_FUNCS([memset])

# Checks for enable/disable Options
#AC_ARG_ENABLE([dams],
#	[AS_HELP_STRING([--enable-dams],
#		[use dams geocoder (default is yes)])],
#		[disable_dams=yes],
#		[disable_dams=no]
#)
#
#AS_IF([test "x$disable_dams" != xyes],
#	    [AC_DEFINE([USE_DAMS], [1], [dams support])]
#)

# List of target config files
AC_CONFIG_FILES([Makefile
                 include/Makefile
                 libgeonlp/Makefile
                 src/Makefile
		 etc/Makefile
                 geonlp_ma_makedic/Makefile
		 dict/Makefile
		 php-extension/Makefile
                 etc/geonlp.rc
                 etc/geonlp_ma_makedic.rc])
AC_OUTPUT
