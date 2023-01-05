#include "User.h"

#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/server_context.h>

#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <nlohmann/json.hpp>
#include <string>

#include "CommandReader.h"
#include "src/protos/online_shopping.pb.h"

using nlohmann::json;

User::User() {
  std::ifstream settings_file("../data/settings.json");
  json settings = json::parse(settings_file);
  settings_file.close();
  auto ip_addresses =
      settings.at("ip_addresses").get<std::map<std::string, std::string>>();
  grpc::ChannelArguments channel_args;
  channel_args.SetInt(GRPC_ARG_ENABLE_HTTP_PROXY, 0);
  for (auto iter = ip_addresses.begin(); iter != ip_addresses.end(); ++iter) {
    clients_.emplace_back(
        iter->first, iter->second,
        grpc::CreateCustomChannel(
            iter->second, grpc::InsecureChannelCredentials(), channel_args));
  }
}

void User::Start() {
  std::string prompt_message =
      "Choose the shop server you want to connect to: \n(";
  for (size_t i = 0; i < clients_.size(); ++i) {
    prompt_message += std::to_string(i + 1) + ": " + clients_[i].server_id();
    if (i != clients_.size() - 1) {
      prompt_message += ", ";
    }
  }
  prompt_message += ") ";
  curr_srv_idx_ = CommandReader::ReadOption(prompt_message, 1, clients_.size());
  --curr_srv_idx_;

  std::cout << "\033[1;32mShop server " << clients_[curr_srv_idx_].server_id()
            << " chosen.\033[0m" << std::endl;

  shop_id_ = clients_[curr_srv_idx_].server_id();

  LoginOrRegister();
}

void User::LoginOrRegister() {
  std::string prompt_message =
      "Do you want to login or register?\n(1: login, 2: register) ";
  size_t login_or_register = CommandReader::ReadOption(prompt_message, 1, 2);

  switch (login_or_register) {
    case 1:
      Login();
      break;
    case 2:
      Register();
      break;
    default:
      LoginOrRegister();
  }
}

bool User::ListItemsInternal() {
  // RPC
  grpc::ClientContext context;
  shopping::ListItemsRequest request;
  request.set_shop_id(shop_id_);
  shopping::ListItemsReply reply;
  grpc::Status status =
      clients_[curr_srv_idx_].stub()->RPCListItems(&context, request, &reply);

  // Connection failed
  if (!status.ok()) {
    if (status.error_code() == grpc::StatusCode::UNAVAILABLE) {
      std::cout << "\033[1;31mServer " << clients_[curr_srv_idx_].server_id()
                << "(" << clients_[curr_srv_idx_].server_address()
                << ") is unavailable.\033[0m" << std::endl;
      if (TryOtherServers()) {
        std::cout << "\033[1;31mAll servers is unavailable, please try again "
                     "later.\033[0m"
                  << std::endl;
      } else {
        std::cout << "\033[1;32mServer " << clients_[curr_srv_idx_].server_id()
                  << "(" << clients_[curr_srv_idx_].server_address()
                  << ") is available, please try again.\033[0m" << std::endl;
      }
    } else {
      std::cout << "\033[1;31mList items failed, please try again.\033[0m"
                << std::endl;
    }
    return true;
  }

  // Handle message
  switch (reply.error()) {
    case shopping::ErrorType::NO_ERROR:
      std::cout << "Items of shop " << shop_id_ << ":\n" << std::endl;
      for (const auto &item : reply.items()) {
        std::cout << "ID: " << item.item_id() << "\tName: " << item.item_name()
                  << "\tPrice: " << item.price() << "\tCount: " << item.count()
                  << std::endl;
      }
      std::cout << std::endl;
      return false;
    case shopping::ErrorType::SHOP_NOT_FOUND:
      std::cout << "\033[1;31mShop " << shop_id_
                << " not found! Try to load shop...\033[0m" << std::endl;
      LoadShopInternal();
      return true;
    default:
      std::cout << "\033[1;31mUnknown error. Please try again.\033[0m"
                << std::endl;
      return true;
  }
}

bool User::LoadShopInternal() {
  // RPC
  grpc::ClientContext context;
  shopping::LoadShopRequest request;
  request.set_shop_id(shop_id_);
  shopping::LoadShopReply reply;
  grpc::Status status =
      clients_[curr_srv_idx_].stub()->RPCLoadShop(&context, request, &reply);

  // Connection failed
  if (!status.ok()) {
    if (status.error_code() == grpc::StatusCode::UNAVAILABLE) {
      std::cout << "\033[1;31mServer " << clients_[curr_srv_idx_].server_id()
                << "(" << clients_[curr_srv_idx_].server_address()
                << ") is unavailable.\033[0m" << std::endl;
      if (TryOtherServers()) {
        std::cout << "\033[1;31mAll servers is unavailable, please try again "
                     "later.\033[0m"
                  << std::endl;
      } else {
        std::cout << "\033[1;32mServer " << clients_[curr_srv_idx_].server_id()
                  << "(" << clients_[curr_srv_idx_].server_address()
                  << ") is available, please try again.\033[0m" << std::endl;
      }
    } else {
      std::cout << "\033[1;31mLoad shop failed, please try again later.\033[0m"
                << std::endl;
    }
    return true;
  }

  // Initialization
  auto iter = clients_.begin();
  std::advance(iter, curr_srv_idx_);
  std::string server_id = iter->server_id();
  grpc::ChannelArguments channel_args;
  channel_args.SetInt(GRPC_ARG_ENABLE_HTTP_PROXY, 0);

  // Handle message
  switch (reply.error()) {
    case shopping::ErrorType::NO_ERROR:
      std::cout << "\033[1;32mShop " << shop_id_
                << " load successfully, please try again.\033[0m" << std::endl;
      return false;
    case shopping::ErrorType::SHOP_ALREADY_RUNNING:
      std::cout << "\033[1;31mShop " << shop_id_
                << "'s original server is running on" << reply.server_address()
                << ". Try to connect...\033[0m" << std::endl;
      iter = clients_.erase(iter);
      clients_.insert(iter, ClientInfo(server_id, reply.server_address(),
                                       grpc::CreateCustomChannel(
                                           reply.server_address(),
                                           grpc::InsecureChannelCredentials(),
                                           channel_args)));
      return false;
    case shopping::ErrorType::SHOP_NOT_FOUND:
      std::cout << "\033[1;31mShop " << shop_id_
                << " not found! Please check if shop's data exists.\033[0m"
                << std::endl;
      return true;
    default:
      std::cout << "\033[1;31mUnknown error. Please try again.\033[0m"
                << std::endl;
      return true;
  }
}

bool User::TryOtherServers() {
  auto srv_idx = curr_srv_idx_;
  auto srv_number = clients_.size();
  bool ok_flag = false;
  while (srv_number != 0) {
    srv_idx = (srv_idx + 1) % clients_.size();
    srv_number--;

    // Try to connect to another server
    // RPC
    grpc::ClientContext context;
    shopping::CheckStatusRequest request;
    shopping::CheckStatusReply reply;
    grpc::Status status =
        clients_[srv_idx].stub()->RPCCheckStatus(&context, request, &reply);

    if (status.ok()) {
      ok_flag = true;
      curr_srv_idx_ = srv_idx;
      break;
    }
  };
  return !ok_flag;
}
