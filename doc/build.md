# yarn
- curl -O -L https://nightly.yarnpkg.com/latest.tar.gz

# postgresql
- curl -O -L https://ftp.postgresql.org/pub/snapshot/dev/postgresql-snapshot.tar.bz2

# nginx
- ./configure --prefix=/g/nginx --with-http_v2_module --with-http_ssl_module --with-mail_ssl_module --with-stream_ssl_module --with-stream_ssl_preread_module
- make -j 4 && make install

# boost
- ./bootstrap.sh --prefix=/g/pkg
- ./b2 install

#libuv
- sh ./autogen.sh
- ./configure --prefix=/g/pkg
- make && make install

#nghttp2
- autoreconf -i
- automake
- autoconf
- ./configure PYTHON=/usr/bin/python3.6 --prefix=/g/pkg --enable-app --enable-examples --with-boost=/g/pkg --enable-asio-lib
- make -j 4 && make install

# curl
- ./buildconf
- ./configure --prefix=/g/pkg --with-nghttp2=/g/pkg
- make && make install

# gtest
- see mai/gtest


