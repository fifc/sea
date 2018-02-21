
// 
// Akago Inc.
// Steen Yi
// 2015-12
//

#define BOOST_NETWORK_ENABLE_HTTPS
#include <boost/program_options.hpp>
#include <boost/network/protocol/http.hpp>
#include <string>
#include <utility>
#include <iostream>

#include "send_sms.h"

namespace sms {
const std::string g_sms_account("jiekou-clcs-15");
const std::string g_sms_passwd("Tch987654");
const std::string g_balance_uri("http://222.73.117.158/msg/QueryBalance");
const std::string g_sms_uri("http://222.73.117.158//msg/HttpBatchSendSM");

static bool IsResponseOK(const std::string& data) {
	auto pos = data.find(',');
	if (pos == std::string::npos) return false;
	auto code_str = data.substr(pos + 1);
	if (code_str.empty()) return false;
	auto code = atol(code_str.c_str());
	// std::cout << "code_str = " << code << ", code = " << code << "\n";
	return code == 0;
}

int SendSM(const std::string& phone, const std::string& msg) {
  using namespace boost::network;

  const bool show_respone = true;
  std::string phone_nr = phone;
  auto pos = phone_nr.find('/');
  if (pos != std::string::npos) {
    phone_nr.erase(pos, 1);
  }
  if (phone_nr.empty()) {
    return -1;
  }
  if (phone_nr[0] != '+') {
    phone_nr = '+' + phone_nr;
  }
  // uri::uri url;
  // url << uri::scheme("http")
  //   << uri::host(g_sms_host)
  //   << uri::path(g_sms_uri)
  //   << uri::query("account=" + uri::encoded(g_sms_account))
  //   << uri::query("pswd=" + uri::encoded(g_sms_passwd))
  //   << uri::query("mobile=" + uri::encoded(phone_nr))
  //   << uri::query("msg=" + uri::encoded(msg))
  //   << uri::query("needstatus=false")
  //   << uri::query("product=")
  //   << uri::query("extno=");

  std::string url_data;
  url_data = "account=" + uri::encoded(g_sms_account);
  url_data += "&mobile=" + uri::encoded(phone_nr);
  url_data += "&product=";
  url_data += "&needstatus=false";
  url_data += "&msg=" + uri::encoded(msg);
  url_data += "&pswd=" + uri::encoded(g_sms_passwd);
  url_data += "&extno=";
  std::cout << "payload data = [" << url_data << "]\n";

  http::client::request request(g_sms_uri);
  request << ::boost::network::header("Connection", "close");
  request << ::boost::network::header("Content-type", "application/x-www-form-urlencoded");
  request << ::boost::network::header("Accept", "text/plain");
  request << ::boost::network::header("Content-Length", std::to_string(url_data.size()));
  request << body(url_data);

  http::client::options client_options;
  client_options.follow_redirects(true);
  http::client client(client_options);
  http::client::response response = client.post(request);

  if (show_respone) {
    std::cout << status(response) << " " << status_message(response) << std::endl;

    auto headers_ = response.headers();
    typedef std::pair<std::string, std::string> header_type;
    for (auto const& header : headers_) {
      std::cout << header.first << ": " << header.second << std::endl;
    }
    std::cout << body(response) << std::endl;
  }

  if (!IsResponseOK(std::string(body(response)))) {
    return -1;
  }

  return 0;
}
} // namespace sms

#ifdef TEST_SEND_SMS
int main(int argc, char *argv[]) {
	std::string phone = argc>1?argv[1]:"186030000506";
	std::string msg =  "您好，您的验证码是12345";
	sms::SendSM(phone, msg);
}
#endif // 0
