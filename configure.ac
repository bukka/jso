# Configuration script for autoconf

AC_PREREQ([2.59])
AC_INIT([jso], [1.0], [jakub.php@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_CONFIG_SRCDIR([src/jso.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([memmove strtol])
AC_CHECK_SIZEOF(long)

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_OFF_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset])

# Check for Bison
AC_PROG_YACC
AS_IF([test "x$YACC" = x], [AC_MSG_ERROR([bison not found])])

# Check for re2c
AC_CHECK_PROG(RE2C, re2c, re2c)
AS_IF([test "x$RE2C" = x], [AC_MSG_ERROR([re2c not found])])

AC_ARG_ENABLE(debug,
  [AS_HELP_STRING([--enable-debug],
				  [Enable debugging])],
  [jso_debug=yes],
  [jso_debug=no])

if test "x$jso_debug" = "xyes"; then
  changequote({,})
  CFLAGS=`echo "$CFLAGS" | $SED -e 's/-O[0-9s]*//g'`
  changequote([,])
  CFLAGS="$CFLAGS -O0 -gdwarf-2 -g3"
fi

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
