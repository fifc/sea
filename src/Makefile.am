bin_PROGRAMS = sea
sea_SOURCES = main.cc proto.pb.cc
sea_CPPFLAGS = # $(grpc_CFLAGS)
sea_LDFLAGS = -L${abs_top_builddir}/lib
sea_LDADD = -lgw -lgrpc # $(grpc_LIBS)

%.pb.cc %.pb.h: ${abs_top_builddir}/src/%.proto
	protoc --proto_path=${abs_top_builddir}/src --cpp_out=. $^
