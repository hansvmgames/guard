#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([guard], [0.1], [hansvm.projects@gmail.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([assert.h stdarg.h stdbool.h stdio.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AX_PTHREAD([], [AC_ERROR([posix threading library not found])])
AC_SEARCH_LIBS([SDL_Init], [SDL2], [], [AC_ERROR([SDL2 library not found])])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
