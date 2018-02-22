/*
 * http2 api test
 *
 * Copyright (c) 2018 Steven Yi
 *
 */

#include <iostream>
#include <memory>

#include <nghttp2/asio_http2_client.h>
#include "html_test.h"
#include "login_test.h"
#include "getpicvericode_test.h"
#include "smsvericode_test.h"
#include "signup_test.h"

using boost::asio::ip::tcp;

using namespace nghttp2::asio_http2;
using namespace nghttp2::asio_http2::client;
using namespace sea_proto;

#define ADD_TEST(name) \
	if (strcasecmp(test_name, #name) == 0) return std::make_shared<name ## TestCase>()

std::shared_ptr<TestCase> CreateTestCase(const char *test_name) {

	if (test_name == nullptr) {
		return std::make_shared<HtmlTestCase>();
	}

	ADD_TEST(Signup);
	ADD_TEST(Login);
	ADD_TEST(GetPicVericode);
	ADD_TEST(GetSmsVericode);

	return std::make_shared<HtmlTestCase>();
}

int main(int argc, char *argv[]) {
  try {
    auto test = CreateTestCase(argc>2?argv[2]:nullptr);
    std::string uri;
    if (argc > 1 && argv[1][0] != '-') {
      if (strchr(argv[1], '/') == NULL && strchr(argv[1], ':') == NULL) {
	uri = "http://localhost/sea/";
	uri += argv[1];
      } else {
        if (strstr(argv[1], "://") == NULL) {
          uri = "http://";
	  uri += argv[1];
        } else {
          uri = argv[1];
        }
      }
    } else {
      uri = test->GetUri();
    }

    boost::system::error_code ec;
    boost::asio::io_service io_service;

    std::string scheme, host, service;

    if (host_service_from_uri(ec, scheme, host, service, uri)) {
      std::cerr << "error: bad URI: " << ec.message() << std::endl;
      return 1;
    }

    std::cout << "url: " << uri << "\n";

    boost::asio::ssl::context tls_ctx(boost::asio::ssl::context::sslv23);
    tls_ctx.set_default_verify_paths();
    // disabled to make development easier...
    tls_ctx.set_verify_mode(boost::asio::ssl::verify_peer);
    configure_tls_context(ec, tls_ctx);

    auto sess = scheme == "https" ? session(io_service, tls_ctx, host, service)
                                  : session(io_service, host, service);
    std::cout << "connecting to " << host << ":" << service << "\n";

    sess.on_connect([&sess, &uri, test](tcp::resolver::iterator endpoint_it) {
      boost::system::error_code ec;
      auto content = test->GenReq();
      auto req = sess.submit(ec, "POST", uri, content, {{"content-type",{"application/octet-stream"}}});

      if (ec) {
        std::cerr << "error: " << ec.message() << std::endl;
        return;
      }

      req->on_response([test, &sess](const response &res) {
        std::cerr << "HTTP/2 " << res.status_code() << std::endl;
        for (auto &kv : res.header()) {
          std::cerr << kv.first << ": " << kv.second.value << "\n";
        }
        std::cerr << std::endl;
	test->OnResponse();
	res.on_data([test, &sess](const uint8_t *data, std::size_t len) {
	  test->OnData((char *)data, len);
	});
      });

      req->on_close([&sess, test](uint32_t error_code) {
	std::cerr << "request stream closed\n";
        test->Report();
      });
    });

    sess.on_error([](const boost::system::error_code &ec) {
      std::cerr << "error: " << ec.message() << std::endl;
    });

    io_service.run();
    sess.shutdown();
  } catch (std::exception &e) {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}
