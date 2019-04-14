#include <iostream>


#include <cxxopts.hpp>
#include "client.hpp"

cxxopts::Options options() {

  try {
    cxxopts::Options options("Agar.io Client", "command line options");
    options
      .positional_help("[optional args]")
      .show_positional_help();

    options.add_options()
      ("s,standalone", "standalone mode", cxxopts::value<bool>()->default_value("false"))
      ("i,server", "Server", cxxopts::value<std::string>()->default_value("localhost"))
      ("port", "Port", cxxopts::value<int>()->default_value("80"))
      ("help", "Print help");


    options.parse_positional({"server"});
    return options;

  } catch (const cxxopts::OptionException &e) {
    std::cout << "error parsing options: " << e.what() << std::endl;
    exit(1);
  }
}


int main(int argc, char *argv[]) {
  auto opts = options();
  auto args = opts.parse(argc, argv);

  bool standalone = args["standalone"].as<bool>();

  if (standalone) {
    std::cout << "Standalone mode." << std::endl;

    agario::Client client;
    client.set_player(0, "Player1");
    client.game_loop();

  } else {
    std::string server = args["server"].as<std::string>();
    int port = args["port"].as<int>();

    std::cout << "Server: " << server << std::endl;
    std::cout << "Port: " << port << std::endl;

    agario::Client client(server, port);
    client.connect();
    client.game_loop();
  }

  return 0;
}
