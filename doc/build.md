# jemalloc
	- ./autogen.sh
	- make dist
	- make
	- make install
# boost
	- ./bootstrap.sh --prefix=/g/pkg
	- ./b2 variant=release install

# libuv
	- sh ./autogen.sh
	- ./configure --prefix=/g/pkg
	- make && make install

# nghttp2
	- rm -rf /g/pkg/include/nghttp2 /g/pkg/lib/libnghttp2*
	- autoreconf -i
	- automake
	- autoconf
	- ./configure PYTHON=/usr/bin/python3 --prefix=/g/pkg --enable-app --enable-examples --with-boost=/g/pkg --enable-asio-lib
	- make -j 4 && make install

# curl
	- rm -rf /g/pkg/include/curl /g/pkg/lib/libcurl.*
	- ./buildconf
	- ./configure --prefix=/g/pkg --with-nghttp2=/g/pkg --with-libssh2
	- make && make install

# gtest
	- see sea/gtest

# gflags
	- cmake -DCMAKE_INSTALL_PREFIX=/g/pkg -DBUILD_SHARED_LIBS=ON ..

# glog:
	- ./configure --with-gflags=/g/pkg --prefix=/g/pkg

# libwebsockets:
	- rm -rf /g/pkg/include/libwebsockets.h /g/pkg/lib/libwebsockets.*
	- cmake .. -DCMAKE_INSTALL_PREFIX=/g/pkg  -DLWS_IPV6=ON -DLWS_WITH_LIBUV=ON -DLWS_LIBUV_LIBRARIES=/g/pkg/lib/libuv.so -DLWS_LIBUV_INCLUDE_DIRS=/g/pkg/include
	- make VERBOSE=1

# postgresql
	- rm -rf /g/pkg/include/postgres* /g/pkg/lib/libpq.* /g/pkg/lib/libpg*
	- curl -O -L https://ftp.postgresql.org/pub/snapshot/dev/postgresql-snapshot.tar.bz2
	- ./configure --prefix=/g/pkg --with-openssl

# nginx
	- ./configure --prefix=/g/nginx --with-http_v2_module --with-http_ssl_module --with-mail_ssl_module --with-stream_ssl_module --with-stream_ssl_preread_module
	- make -j 4 && make install

# yarn
	- curl -O -L https://nightly.yarnpkg.com/latest.tar.gz
	- ./configure --prefix=/g/pkg --with-openssl
	- make -j 4 && make install

# redis
	- curl -L -O https://github.com/antirez/redis/archive/unstable.tar.gz
	- make
	- make PREFIX=/g/pkg install

# hiredis
	- make PREFIX=/g/pkg install 

# mosquitto
	- config.mk: prefix=/g/pkg WITH_WEBSOCKETS:=yes
	- make CFLAGS=-I/g/pkg/include LDFLAGS="-L/g/pkg/lib -luv"
	- make install
	- mkdir /g/pkg/etc && mv /etc/mosquitto /g/pkg/etc/

# protobuf
	- rm -rf /g/pkg/include/google/protobuf /g/pkg/lib/libprotobuf* /g/pkg/lib/libprotoc.*
	- ./autogen.sh -i
	- ./configure --prefix=/g/pkg
	- make -j 5 && make install

# grpc
	- rm -rf /g/pkg/include/grpc /g/pkg/lib/libgrpc*
	- git submodule update --init (or: git pull --recurse-submodules)
	- make prefix=/g/pkg CFLAGS=-I/g/pkg/include CXXFLAGS=-I/g/pkg/include LDFLAGS=-L/g/pkg/lib install

# yaml-cpp
	- rm -rf /g/pkg/include/yaml-cpp /g/pkg/lib/libyaml*
	- mkdir build && cd build
	- cmake -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=/g/pkg ..

# jsoncpp
	- export DESTDIR=/g/pkg
	- make build && cd build
	- meson --buildtype release --default-library shared .. build-release
	- ninja -v -C build-release install
	- move it from $DESTDIR/usr/local to $DESTDIR

# ss-libev
	- ./configure CC=gcc-8 --prefix=/opt --disable-documentation

