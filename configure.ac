AC_INIT([sea], [1.0], [y@ffox.top])
dnl AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_PROG_CC
AC_PROG_CXX
AM_PROG_AR
AC_CONFIG_HEADERS([config.h])

LT_INIT

CPPFLAGS+=-l/g/pkg/include
LDFLAGS+=-L/g/pkg/lib

dnl PKG_PROG_PKG_CONFIG
dnl AC_CHECK_LIB([boost_chrono], [sdfs], [], [AC_MSG_ERROR()])
dnl PKG_CHECK_MODULES([grpc], [grpc])
dnl PKG_CHECK_MODULES([protobuf], [protobuf])

AC_CONFIG_FILES([
 Makefile
 src/Makefile
 lib/Makefile
])
AC_OUTPUT
