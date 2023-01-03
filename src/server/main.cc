#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

#include "RPCServer.h"

using json = nlohmann::json;

void RunServer(const std::string& server_id,
               const std::string& server_address) {
  RPCServerImpl service(server_id);

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}

int main(int argc, char** argv) {
  std::ifstream settings_file("../data/settings.json");
  json settings = json::parse(settings_file);
  settings_file.close();
  auto ip_addresses = settings.at("ip_addresses")
                          .get<std::unordered_map<std::string, std::string>>();

  if (argc != 2) {
    std::cout << "Usage: ./server [server_id]" << std::endl;
    return 0;
  } else {
    std::string server_id = argv[1];
    if (ip_addresses.find(server_id) == ip_addresses.end()) {
      std::cout << "Invalid server name! Available server names: " << std::endl;
      for (auto iter = ip_addresses.begin(); iter != ip_addresses.end();
           ++iter) {
        std::cout << '\t' << iter->first << std::endl;
      }
      return 0;
    } else {
      RunServer(server_id, ip_addresses[server_id]);
    }
  }

  return 0;
}
