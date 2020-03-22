# jemalloc
	- ./autogen.sh
	- ./configure --prefix=/g/ex
	- make install

# boost
	- rm -rf /g/rt/include/boost /g/rt/lib/libboost_*
	- ./bootstrap.sh --prefix=/g/rt
	- ./b2 variant=release install

# libuv
	- rm -rf /g/rt/include/uv-* /g/rt/include/uv.h /g/rt/lib/libuv.*
	- sh ./autogen.sh
	- ./configure --prefix=/g/rt
	- make && make install

# nghttp2
	- rm -rf /g/rt/include/nghttp2 /g/rt/lib/libnghttp2* /g/rt/bin/nghttp*
	- autoreconf -i
	- automake
	- autoconf
	- ./configure PYTHON=/usr/bin/python3 --prefix=/g/rt --enable-app --enable-examples --with-boost=/g/rt --enable-asio-lib
	- make -j 4 && make install

# curl
	- rm -rf /g/rt/include/curl /g/rt/lib/libcurl.* /g/rt/bin/curl
	- ./buildconf
	- ./configure --prefix=/g/rt --with-nghttp2=/g/rt --with-libssh2
	- make && make install

# gtest
	- see sea/gtest

# gflags
	- rm -rf /g/rt/include/gflags /g/rt/lib/libgflags*
	- cmake -DCMAKE_INSTALL_PREFIX=/g/rt -DBUILD_SHARED_LIBS=ON ..

# glog:
	- rm -rf /g/rt/include/glog /g/rt/lib/libglog.*
	- ./configure --with-gflags=/g/rt --prefix=/g/rt

# libwebsockets:
	- rm -rf /g/rt/include/libwebsockets.h /g/rt/lib/libwebsockets.*
	- cmake .. -DCMAKE_INSTALL_PREFIX=/g/rt  -DLWS_IPV6=ON -DLWS_WITH_LIBUV=ON -DLWS_LIBUV_LIBRARIES=/g/rt/lib/libuv.so -DLWS_LIBUV_INCLUDE_DIRS=/g/rt/include
	- make VERBOSE=1

# postgresql
	- rm -rf /g/rt/include/postgres* /g/rt/lib/libpq.* /g/rt/lib/libpg* /g/rt/bin/psql /g/rt/bin/initdb /g/rt/bin/pgbench /g/rt/bin/pg_*
	- curl -OL https://ftp.postgresql.org/pub/snapshot/dev/postgresql-snapshot.tar.bz2
	- ./configure --prefix=/g/rt --with-openssl

# nginx
	- ./configure --prefix=/g/nginx --with-http_v2_module --with-http_ssl_module --with-mail_ssl_module --with-stream_ssl_module --with-stream_ssl_preread_module
	- make -j 4 && make install

# yarn
	- curl -OL https://nightly.yarnpkg.com/latest.tar.gz
	- ./configure --prefix=/g/rt --with-openssl
	- make -j 4 && make install

# redis
	- curl -LO https://github.com/antirez/redis/archive/unstable.tar.gz
	- make
	- make PREFIX=/g/rt install

# hiredis
	- make PREFIX=/g/rt install 

# mosquitto
	- rm -rf  /g/rt/include/mosquitto* /g/rt/lib/libmosquitto*
	- git checkout -b remotes/origin/develop
	- config.mk: prefix=/g/rt, WITH_WEBSOCKETS:=yes
	- make CFLAGS=-I/g/rt/include LDFLAGS="-L/g/rt/lib -luv -ljemalloc" -j 4
	- make install
	- mkdir /g/rt/etc && mv /etc/mosquitto /g/rt/etc/

# protobuf
	- rm -rf /g/rt/include/google/protobuf /g/rt/lib/libprotobuf* /g/rt/lib/libprotoc.*
	- ./autogen.sh -i
	- ./configure --prefix=/g/rt
	- make -j 5 && make install

# grpc
	- rm -rf /g/rt/include/grpc /g/rt/lib/libgrpc*
	- git submodule update --init (or: git pull --recurse-submodules)
	- make prefix=/g/rt CFLAGS=-I/g/rt/include CXXFLAGS=-I/g/rt/include LDFLAGS=-L/g/rt/lib install

# yaml-cpp
	- rm -rf /g/rt/include/yaml-cpp /g/rt/lib/libyaml*
	- mkdir build && cd build
	- cmake -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=/g/rt ..

# jsoncpp
	- export DESTDIR=/g/rt
	- mkdir build && cd build
	- meson --buildtype release --default-library shared .. build-release
	- ninja -v -C build-release install
	- move it from $DESTDIR/usr/local to $DESTDIR

# ss-libev
	- ./configure CC=gcc-10 --prefix=/opt --disable-documentation

# librdkafka
	- apt install liblz4-dev libsasl2-dev
	- ./configure --prefix=/g/rt

# ngtcp2
	- rm -rf /g/rt/include/librdkafka /g/rt/lib/librdkafka*
	- ./configure --prefix=/g/rt CFLAGS=-I/g/rt/include CXXFLAGS=-I/g/rt/include LDFLAGS=-L/g/rt/lib\ -Wl,-rpath,/g/ngtcp2/openssl/build/lib PKG_CONFIG_PATH=/g/ngtcp2/openssl/build/lib/pkgconfig
	- make -j4 # or: make -j$(nproc)

# cassandra cpp-driver
	- cmake .. -DCMAKE_INSTALL_PREFIX=/g/rt
	- make -j 4
	- make install
