lib_LIBRARIES = libsea.a
libsea_a_SOURCES = sea.cc
libsea_a_CPPFLAGS = #$(grpc_CFLAGS)
libsea_a_CXXFLAGS = -pthread -I${abs_top_srcdir}/lib
libsea_a_LIBADD = #libprotobuf.so #$(grpc_LIBS)

lib_LTLIBRARIES = libgw.la
libgw_la_SOURCES = gw.cc
include_HEADERS = gw.h
libgw_la_CXXFLAGS = -pthread -I${abs_top_srcdir}/lib
libgw_la_LDFLAGS = -pthread -L${abs_top_builddir}/lib

libgw_la_LIBADD = -lprotobuf
