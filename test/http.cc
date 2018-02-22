
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

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  using namespace boost::network;

  uri::uri instance("http://cpp-netlib.github.com/");
  instance << uri::query(uri::encoded("a-b=c d ? aa"));
  assert(instance.is_valid());
  std::cout << "scheme: " << instance.scheme() << std::endl
	  << "host: " << instance.host() << std::endl
	  << "query: " << instance.query() << std::endl
	  << "string: " << instance.string() << std::endl;


  po::options_description options("Allowed options");
  std::string output_filename, source;
  bool show_headers;
  options.add_options()("help,h", "produce help message")(
      "headers,H", "print headers")("status,S", "print status and message")(
      "source,s", po::value<std::string>(&source), "source URL");

  po::positional_options_description positional_options;
  positional_options.add("source", 1);
  po::variables_map vm;
  try {
    po::store(po::command_line_parser(argc, argv)
                  .options(options)
                  .positional(positional_options)
                  .run(),
              vm);
    po::notify(vm);
  }
  catch (std::exception& e) {
    std::cout << "Error: " << e.what() << std::endl;
    std::cout << options << std::endl;
    return EXIT_FAILURE;
  }

  if (vm.count("help")) {
    std::cout << options << std::endl;
    return EXIT_SUCCESS;
  }

  if (vm.count("source") < 1) {
    std::cout << "Error: Source URL required." << std::endl;
    std::cout << options << std::endl;
    return EXIT_FAILURE;
  }

  show_headers = vm.count("headers") ? true : false;
  bool show_status = vm.count("status") ? true : false;

  http::client::request request(instance);
  http::client::string_type destination_ = host(request);

  request << ::boost::network::header("Connection", "close");
  http::client::options client_options;
  client_options.follow_redirects(true);
  http::client client(client_options);
  http::client::response response = client.post(request);

  if (show_status)
    std::cout << status(response) << " " << status_message(response)
              << std::endl;

  if (show_headers) {
    auto headers_ = response.headers();
    typedef std::pair<std::string, std::string> header_type;
    for (auto const& header : headers_) {
      std::cout << header.first << ": " << header.second << std::endl;
    }
    std::cout << std::endl;
  }

  std::cout << body(response);
  return EXIT_SUCCESS;
}
//]
