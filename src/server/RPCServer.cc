#include "RPCServer.h"

#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/support/status.h>

#include <boost/filesystem.hpp>
#include <boost/filesystem/file_status.hpp>
#include <cstdint>
#include <fstream>
#include <nlohmann/json.hpp>

#include "Shop.h"
#include "src/protos/online_shopping.pb.h"

using json = nlohmann::json;

RPCServer::RPCServer(const std::string &shop_id,
                     const std::string &server_address)
    : server_address_(server_address) {
  shops_[shop_id] = Shop();
  shops_[shop_id].LoadData(shop_id);

  // load managers from json file
  std::ifstream managers_file("../data/managers.json");
  managers_json_ = json::parse(managers_file);

  managers_file.close();
  for (auto &manager_json :
       managers_json_.at("managers").get<std::vector<json>>()) {
    ManagerInfo manager;
    manager_json.at("user_id").get_to(manager.user_id_);
    manager_json.at("password").get_to(manager.password_);
    managers_[manager.user_id_] = manager;
  }
}

grpc::Status RPCServer::RPCLogin(grpc::ServerContext * /*context*/,
                                 const shopping::LoginRequest *request,
                                 shopping::LoginReply *reply) {
  if (request->user_type() == shopping::UserType::MANAGER) {
    if (managers_.find(request->user_id()) == managers_.end()) {
      reply->set_error(shopping::ErrorType::USER_NOT_FOUND);
      return grpc::Status::OK;
    } else if (managers_[request->user_id()].password_ != request->password()) {
      reply->set_error(shopping::ErrorType::WRONG_PASSWORD);
      return grpc::Status::OK;
    } else {
      reply->set_error(shopping::ErrorType::NO_ERROR);
      return grpc::Status::OK;
    }
  } else {
    if (shops_.find(request->shop_id()) == shops_.end()) {
      reply->set_error(shopping::ErrorType::SHOP_NOT_FOUND);
      return grpc::Status::OK;
    } else {
      auto &customers = shops_[request->shop_id()].GetCustomers();
      auto iter = customers.find(request->user_id());
      if (iter == customers.end()) {
        reply->set_error(shopping::ErrorType::USER_NOT_FOUND);
        return grpc::Status::OK;
      } else if (iter->second.password_ != request->password()) {
        reply->set_error(shopping::ErrorType::WRONG_PASSWORD);
        return grpc::Status::OK;
      } else {
        reply->set_error(shopping::ErrorType::NO_ERROR);
        reply->set_balance(iter->second.balance_);
        return grpc::Status::OK;
      }
    }
  }
}

grpc::Status RPCServer::RPCRegister(grpc::ServerContext * /*context*/,
                                    const shopping::RegisterRequest *request,
                                    shopping::RegisterReply *reply) {
  if (request->user_type() == shopping::UserType::MANAGER) {
    if (managers_.find(request->user_id()) != managers_.end()) {
      reply->set_error(shopping::ErrorType::USER_ALREADY_EXISTS);
      return grpc::Status::OK;
    } else {
      reply->set_error(shopping::ErrorType::NO_ERROR);
      managers_[request->user_id()] = {request->user_id(), request->password()};
      managers_json_.at("managers")
          .push_back({{"user_id", request->user_id()},
                      {"password", request->password()}});
      SaveManagers();
      return grpc::Status::OK;
    }
  } else {
    if (shops_.find(request->shop_id()) == shops_.end()) {
      reply->set_error(shopping::ErrorType::SHOP_NOT_FOUND);
      return grpc::Status::OK;
    } else {
      auto &customers = shops_[request->shop_id()].GetCustomers();
      if (customers.find(request->user_id()) != customers.end()) {
        reply->set_error(shopping::ErrorType::USER_ALREADY_EXISTS);
        return grpc::Status::OK;
      } else {
        reply->set_error(shopping::ErrorType::NO_ERROR);
        reply->set_balance(0);
        shops_[request->shop_id()].SetCustomer(
            {request->user_id(), request->password(), 0});
        shops_[request->shop_id()].SaveData(request->shop_id());
        return grpc::Status::OK;
      }
    }
  }
}

grpc::Status RPCServer::RPCListItems(grpc::ServerContext * /*context*/,
                                     const shopping::ListItemsRequest *request,
                                     shopping::ListItemsReply *reply) {
  if (shops_.find(request->shop_id()) == shops_.end()) {
    reply->set_error(shopping::ErrorType::SHOP_NOT_FOUND);
    return grpc::Status::OK;
  } else {
    reply->set_error(shopping::ErrorType::NO_ERROR);
    auto &items = shops_[request->shop_id()].GetItems();
    for (auto &item : items) {
      auto item_info = reply->add_items();
      item_info->set_item_id(item.second.item_id());
      item_info->set_item_name(item.second.item_name());
      item_info->set_price(item.second.price());
      item_info->set_count(item.second.count());
    }
    return grpc::Status::OK;
  }
}

grpc::Status RPCServer::RPCPurchase(grpc::ServerContext * /*context*/,
                                    const shopping::PurchaseRequest *request,
                                    shopping::PurchaseReply *reply) {
  if (shops_.find(request->shop_id()) == shops_.end()) {
    reply->set_error(shopping::ErrorType::SHOP_NOT_FOUND);
    return grpc::Status::OK;
  } else {
    auto &customers = shops_[request->shop_id()].GetCustomers();
    auto customer_iter = customers.find(request->user_id());
    if (customer_iter == customers.end()) {
      reply->set_error(shopping::ErrorType::USER_NOT_FOUND);
      return grpc::Status::OK;
    } else {
      auto &items = shops_[request->shop_id()].GetItems();
      auto item_iter = items.find(request->item_id());
      if (item_iter == items.end()) {
        reply->set_error(shopping::ErrorType::ITEM_NOT_FOUND);
        return grpc::Status::OK;
      } else if (item_iter->second.count() < request->count()) {
        reply->set_error(shopping::ErrorType::ITEM_NOT_ENOUGH);
        return grpc::Status::OK;
      } else if (customer_iter->second.balance_ <
                 item_iter->second.price() * request->count()) {
        reply->set_error(shopping::ErrorType::INSUFFICIENT_BALANCE);
        return grpc::Status::OK;
      } else {
        double new_balance = customer_iter->second.balance_ -
                             item_iter->second.price() * request->count();
        shopping::Item *new_item = new shopping::Item;
        new_item->set_item_id(item_iter->second.item_id());
        new_item->set_item_name(item_iter->second.item_name());
        new_item->set_price(item_iter->second.price());
        new_item->set_count(item_iter->second.count() - request->count());

        reply->set_error(shopping::ErrorType::NO_ERROR);
        reply->set_allocated_item(new_item);
        reply->set_balance(new_balance);

        shops_[request->shop_id()].SetCustomer(
            {request->user_id(), customer_iter->second.password_, new_balance});
        shops_[request->shop_id()].SetItem(*new_item);
        shops_[request->shop_id()].SaveData(request->shop_id());
        return grpc::Status::OK;
      }
    }
  }
}

grpc::Status RPCServer::RPCEditOrAddItem(
    grpc::ServerContext * /*context*/,
    const shopping::EditOrAddItemRequest *request,
    shopping::EditOrAddItemReply *reply) {
  if (shops_.find(request->shop_id()) == shops_.end()) {
    reply->set_error(shopping::ErrorType::SHOP_NOT_FOUND);
    return grpc::Status::OK;
  } else {
    reply->set_error(shopping::ErrorType::NO_ERROR);
    reply->set_is_added(shops_[request->shop_id()].SetItem(request->item()));
    shops_[request->shop_id()].SaveData(request->shop_id());
    return grpc::Status::OK;
  }
}

grpc::Status RPCServer::RPCDeleteItem(
    grpc::ServerContext * /*context*/,
    const shopping::DeleteItemRequest *request,
    shopping::DeleteItemReply *reply) {
  if (shops_.find(request->shop_id()) == shops_.end()) {
    reply->set_error(shopping::ErrorType::SHOP_NOT_FOUND);
    return grpc::Status::OK;
  } else {
    auto &items = shops_[request->shop_id()].GetItems();
    if (items.find(request->item_id()) == items.end()) {
      reply->set_error(shopping::ErrorType::ITEM_NOT_FOUND);
      return grpc::Status::OK;
    } else {
      reply->set_error(shopping::ErrorType::NO_ERROR);
      shops_[request->shop_id()].DeleteItem(request->item_id());
      shops_[request->shop_id()].SaveData(request->shop_id());
      return grpc::Status::OK;
    }
  }
}

grpc::Status RPCServer::RPCMoreBalance(
    grpc::ServerContext * /*context*/,
    const shopping::MoreBalanceRequest *request,
    shopping::MoreBalanceReply *reply) {
  if (shops_.find(request->shop_id()) == shops_.end()) {
    reply->set_error(shopping::ErrorType::SHOP_NOT_FOUND);
    return grpc::Status::OK;
  } else {
    auto &customers = shops_[request->shop_id()].GetCustomers();
    auto customer_iter = customers.find(request->user_id());
    if (customer_iter == customers.end()) {
      reply->set_error(shopping::ErrorType::USER_NOT_FOUND);
      return grpc::Status::OK;
    } else {
      double new_balance =
          customer_iter->second.balance_ + request->more_balance();
      reply->set_error(shopping::ErrorType::NO_ERROR);
      reply->set_balance(new_balance);
      shops_[request->shop_id()].SetCustomer({customer_iter->second.user_id_,
                                              customer_iter->second.password_,
                                              new_balance});
      shops_[request->shop_id()].SaveData(request->shop_id());
      return grpc::Status::OK;
    }
  }
}

grpc::Status RPCServer::RPCLoadShop(grpc::ServerContext * /*context*/,
                                    const shopping::LoadShopRequest *request,
                                    shopping::LoadShopReply *reply) {
  std::string shop_filepath = "../data/shop_" + request->shop_id() + ".json";
  boost::filesystem::path f(shop_filepath);
  if (shops_.find(request->shop_id()) == shops_.end() &&
      !boost::filesystem::exists(f)) {
    // No data and can't load data
    reply->set_error(shopping::ErrorType::SHOP_NOT_FOUND);
    return grpc::Status::OK;
  } else {
    std::ifstream settings_file("../data/settings.json");
    json settings = json::parse(settings_file);
    settings_file.close();
    auto ip_addresses =
        settings.at("ip_addresses")
            .get<std::unordered_map<std::string, std::string>>();
    auto ip_iter = ip_addresses.find(request->shop_id());
    if (ip_iter != ip_addresses.end() && ip_iter->second != server_address_) {
      // There is a original server
      std::vector<std::string> shop_ids;  // ignore this
      if (!TestServer(ip_iter->second, shop_ids)) {
        // Target server is available
        reply->set_error(::shopping::ErrorType::SHOP_ALREADY_RUNNING);
        reply->set_server_address(ip_iter->second);
        return grpc::Status::OK;
      }
    }
    if (shops_.find(request->shop_id()) == shops_.end()) {
      shops_[request->shop_id()] = Shop();
      shops_[request->shop_id()].LoadData(request->shop_id());
    }
    reply->set_error(::shopping::ErrorType::NO_ERROR);
    return grpc::Status::OK;
  }
}

grpc::Status RPCServer::RPCCheckStatus(
    grpc::ServerContext * /*context*/,
    const shopping::CheckStatusRequest * /*request*/,
    shopping::CheckStatusReply *reply) {
  for (auto &shop : shops_) {
    reply->add_shop_ids(shop.first);
  }
  return grpc::Status::OK;
}

bool RPCServer::TestServer(const std::string &server_address,
                           std::vector<std::string> &shop_ids) {
  grpc::ChannelArguments channel_args;
  channel_args.SetInt(GRPC_ARG_ENABLE_HTTP_PROXY, 0);
  std::unique_ptr<shopping::OnlineShopping::Stub> stub =
      shopping::OnlineShopping::NewStub(grpc::CreateCustomChannel(
          server_address, grpc::InsecureChannelCredentials(), channel_args));
  // RPC
  grpc::ClientContext context;
  shopping::CheckStatusRequest request;
  shopping::CheckStatusReply reply;
  grpc::Status status = stub->RPCCheckStatus(&context, request, &reply);

  // Connection failed
  if (!status.ok()) {
    return true;
  }

  // Connection success
  for (auto &shop_id : reply.shop_ids()) {
    shop_ids.push_back(shop_id);
  }
  return false;
}

void RPCServer::SaveManagers() {
  std::ofstream managers_file("../data/managers.json");
  managers_file << managers_json_.dump(4);
  managers_file.close();
}