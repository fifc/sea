bin_PROGRAMS = sea
sea_SOURCES = sea.pb.cc sea.grpc.pb.cc main.cc
include_HEADERS=sea.pb.h sea.grpc.pb.h
sea_CPPFLAGS =
sea_CXXFLAGS = -pthread -I${abs_top_srcdir}/lib
sea_LDFLAGS = -pthread -L${abs_top_builddir}/lib
sea_LDADD = -lgw -lgrpc -lgrpc++ -lprotobuf -lpthread

include ${abs_top_srcdir}/../proto/rules.mk
