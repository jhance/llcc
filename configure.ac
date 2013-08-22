AC_PREREQ([2.68])
AC_INIT([llcc], [1.0-gamma], [jaredhance@gmail.com])

AC_CONFIG_AUX_DIR([config])
AC_CONFIG_SRCDIR([llcc/llcc.c])

AM_INIT_AUTOMAKE([foreign])

# Configuration Options
AM_SILENT_RULES

# Outputs
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile llcc/Makefile])

# Checks for programs.
AC_PROG_CC
AC_PROG_LEX
AC_PROG_YACC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADER([stdio.h])
AC_CHECK_HEADER([stdlib.h])
AC_CHECK_HEADER([unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_OUTPUT
