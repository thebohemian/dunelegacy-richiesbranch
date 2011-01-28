AC_INIT(dunelegacy, 0.96.1, http://dunelegacy.sourceforge.net)

AM_INIT_AUTOMAKE

AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CXX

if test "$prefix" = "" ; then
	dunelegacydatadir='data'
else
	dunelegacydatadir='${pkgdatadir}'
fi

AC_SUBST(dunelegacydatadir)

dnl Some flags for gcc
CXXFLAGS="-O3 -pipe -pedantic -ansi -Wall -Wextra -Wno-unused-parameter"

AC_ARG_ENABLE([debug],
            [AS_HELP_STRING([--enable-debug],
              [compile with debug information @<:@default=disabled@:>@])],
            [],
            [])

if test "$enable_debug" = "yes" ; then
	CXXFLAGS="$CXXFLAGS -g"
fi

dnl Check for SDL library
SDL_VERSION=1.2.0
AM_PATH_SDL($SDL_VERSION, : , AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!]))
CFLAGS="$CFLAGS $SDL_CFLAGS"
CXXFLAGS="$CXXFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"

dnl Check for SDL_mixer library.
AC_CHECK_LIB(SDL_mixer, 
       Mix_OpenAudio, 
       LIBS="$LIBS -lSDL_mixer", 
       AC_MSG_ERROR([*** SDL_mixer library not found!]) 
)

AC_OUTPUT(Makefile src/Makefile tests/Makefile)
