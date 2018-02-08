bin_PROGRAMS = sea
sea_SOURCES = main.cc sea.pb.cc sea.grpc.pb.cc
include_HEADERS=sea.pb.h sea.grpc.pb.h
sea_CPPFLAGS =
sea_LDFLAGS = -L${abs_top_builddir}/lib
sea_LDADD = -lgw -lgrpc -lgrpc++ -lprotobuf

include ${abs_top_builddir}/proto/rules.mk
