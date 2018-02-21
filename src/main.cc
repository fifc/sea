/*
 * main.cc
 *
 */

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>

#include <nghttp2/asio_http2_server.h>

#include "handler.h"
#include "cfg.h"
#include "picode.h"
#include "mimetypes.h"

using namespace nghttp2::asio_http2;
using namespace nghttp2::asio_http2::server;

static void init_req_handler(const SeaHandler *handler, http2 *server) {
    for (auto& item : handler->handler_map_) { // binary interface
      std::string prefix = "/sea/" + item.first;
      server->handle(prefix, item.second);
      std::cout << __func__ << " adding " << prefix << "\n";
    }
    for (auto& item : handler->json_handler_map_) { // json interface
      std::string prefix = "/j/" + item.first;
      server->handle(prefix, item.second);
      std::cout << __func__ << " adding " << prefix << "\n";
    }
    server->handle("/", [](const request &req, const response &res) {
	  //for (auto &kv : req.header()) {
	  //  std::cerr << kv.first << ": " << kv.second.value << "\n";
	  //}
	  //std::cerr << std::endl;

      auto path = req.uri().path;
      if (path.empty() || path == "/")
        path = "index.html";
      auto mimetype = get_mimetype(path);
      if (mimetype.empty()) {
        res.write_head(404);
        res.end();
	return;
      }

      if (path.c_str()[0] == '/')
	path = path.substr(1);
      std::ifstream t(path);
      if (t.is_open()) {
	std::string data((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        res.write_head(200, {{"Content-Type", {mimetype}}});
        res.end(data);
      } else {
        res.write_head(404);
        res.end();
      }
      //std::string data = req.uri().raw_query + "\n";
    });
    server->handle("/vericode/", [](const request &req, const response &res) {
      res.write_head(200, {{"Content-Type", {"image/png"}}});
      std::string img;
      CreatePicVericode(&img);
      res.end(img);
    });
    server->handle("/secret/", [](const request &req, const response &res) {
      res.write_head(301);
      res.end("aha got it?!\n");
    });
    server->handle("/push", [](const request &req, const response &res) {
      boost::system::error_code ec;
      auto push = res.push(ec, "GET", "/push/1");
      if (!ec) {
        push->write_head(200);
        push->end("server push FTW!\n");
      }

      res.write_head(200);
      res.end("you'll receive server push!\n");
    });
    server->handle("/delay", [](const request &req, const response &res) {
      res.write_head(200);

      auto timer = std::make_shared<boost::asio::deadline_timer>(
          res.io_service(), boost::posix_time::seconds(3));
      auto closed = std::make_shared<bool>();

      res.on_close([timer, closed](uint32_t error_code) {
        timer->cancel();
        *closed = true;
      });

      timer->async_wait([&res, closed](const boost::system::error_code &ec) {
        if (ec || *closed) {
          return;
        }

        res.end("finally!\n");
      });
    });
    server->handle("/trailer", [](const request &req, const response &res) {
      // send trailer part.
      res.write_head(200, {{"trailers", {"digest"}}});

      std::string body = "nghttp2 FTW!\n";
      auto left = std::make_shared<size_t>(body.size());

      res.end([&res, body, left](uint8_t *dst, std::size_t len,
                                 uint32_t *data_flags) {
        auto n = std::min(len, *left);
        std::copy_n(body.c_str() + (body.size() - *left), n, dst);
        *left -= n;
        if (*left == 0) {
          *data_flags |=
              NGHTTP2_DATA_FLAG_EOF | NGHTTP2_DATA_FLAG_NO_END_STREAM;
          // RFC 3230 Instance Digests in HTTP.  The digest value is
          // SHA-256 message digest of body.
          res.write_trailer(
              {{"digest",
                {"SHA-256=qqXqskW7F3ueBSvmZRCiSwl2ym4HRO0M/pvQCBlSDis="}}});
        }
        return n;
      });
    });
}

int main(int argc, char *argv[]) {
  cfg::init(argc, argv);
  if (isatty(fileno(stdout))) {
	std::string cl = "\33[0;32m";
	std::string el = "\33[m";
	std::cout << "port:" << cl << FLAGS_port << el << ", key: " << cl << FLAGS_key << el << ", cert: " << cl << FLAGS_cert << el << std::endl;
  }

  SeaHandler *handler = nullptr;
  try {
    boost::system::error_code ec;

    http2 server;
    server.num_threads(FLAGS_threads);
    
    handler = new SeaHandler;
    init_req_handler(handler, &server);

    if (!FLAGS_key.empty() && !FLAGS_cert.empty()) {
      boost::asio::ssl::context tls(boost::asio::ssl::context::sslv23);
      tls.use_private_key_file(FLAGS_key, boost::asio::ssl::context::pem);
      tls.use_certificate_chain_file(FLAGS_cert);

      configure_tls_context_easy(ec, tls);

      if (server.listen_and_serve(ec, tls, FLAGS_addr, FLAGS_port)) {
        std::cerr << "error: " << ec.message() << std::endl;
      }
    } else {
      if (server.listen_and_serve(ec, FLAGS_addr, FLAGS_port)) {
        std::cerr << "error: " << ec.message() << std::endl;
      }
    }
  } catch (std::exception &e) {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}
