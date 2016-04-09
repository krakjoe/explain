dnl $Id$
dnl config.m4 for extension explain

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

PHP_ARG_ENABLE(explain, whether to enable explain support,
[  --enable-explain           Enable explain support], yes, yes)

if test "$PHP_EXPLAIN" != "no"; then

  dnl Check PHP version:
  AC_MSG_CHECKING(PHP version)
  if test ! -z "$phpincludedir"; then
    PHP_VERSION=`grep 'PHP_VERSION ' $phpincludedir/main/php_version.h | sed -e 's/.*"\([[0-9\.]]*\)".*/\1/g' 2>/dev/null`
  elif test ! -z "$PHP_CONFIG"; then
    PHP_VERSION=`$PHP_CONFIG --version 2>/dev/null`
  fi

  if test x"$PHP_VERSION" = "x"; then
    AC_MSG_WARN([not found])
  else
    PHP_MAJOR_VERSION=`echo $PHP_VERSION | sed -e 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\).*/\1/g' 2>/dev/null`
    PHP_MINOR_VERSION=`echo $PHP_VERSION | sed -e 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\).*/\2/g' 2>/dev/null`
    PHP_RELEASE_VERSION=`echo $PHP_VERSION | sed -e 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\).*/\3/g' 2>/dev/null`
    AC_MSG_RESULT([$PHP_VERSION])
  fi

  if test "$PHP_MAJOR_VERSION" -eq 5; then
    PHP_NEW_EXTENSION(explain, explain.c, $ext_shared)
  else
    PHP_NEW_EXTENSION(explain, explain_php7.c, $ext_shared)
  fi
fi
