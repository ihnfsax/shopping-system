#include "RPCServer.h"

#include <grpcpp/support/status.h>

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

RPCServerImpl::RPCServerImpl(const std::string &shop_id) {
  shops_[shop_id] = Shop();
  shops_[shop_id].LoadData(shop_id);
  LoadManagers();
}

grpc::Status RPCServerImpl::GetItems(grpc::ServerContext * /*context*/,
                                     const shopping::GetItemsRequest *request,
                                     shopping::GetItemsReply *response) {
  auto shop_id = request->shop_id();
  if (shops_.find(shop_id) == shops_.end()) {
    return grpc::Status(grpc::StatusCode::NOT_FOUND,
                        "Shop '" + shop_id + "' not found");
  }
  response->set_shop_id(request->shop_id());
  for (const auto &item : shops_[shop_id].GetItems()) {
    response->add_items()->CopyFrom(item.second);
  }
  return grpc::Status::OK;
}

void RPCServerImpl::LoadManagers() {
  // load managers from json file
  std::ifstream managers_file("../data/managers.json");
  auto managers_json =
      json::parse(managers_file).at("managers").get<std::vector<json>>();
  managers_file.close();
  for (auto &manager_json : managers_json) {
    shopping::User manager;
    manager.set_type(shopping::RoleType::MANAGER);
    manager.set_id(manager_json.at("account").get<std::string>());
    manager.set_password(manager_json.at("password").get<std::string>());
    manager.set_shop_id("");
    manager.set_balance(0);
    managers_[manager.id()] = manager;
  }
}