#include "Shop.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

void Shop::LoadData(const std::string& shop_name) {
  std::string shop_filepath = "../data/shop_" + shop_name + ".json";
  std::ifstream shop_file(shop_filepath);
  shop_json_ = json::parse(shop_file);
  shop_file.close();

  // load items from json file
  items_.clear();
  auto items_json = shop_json_.at("items").get<std::vector<json>>();
  for (auto& item_json : items_json) {
    shopping::Item item;
    item.set_item_id(item_json.at("item_id").get<std::string>());
    item.set_item_name(item_json.at("item_name").get<std::string>());
    item.set_price(item_json.at("price").get<double>());
    item.set_count(item_json.at("count").get<uint32_t>());
    items_[item.item_id()] = item;
  }

  // load customer fron json file
  customers_.clear();
  auto customers_json = shop_json_.at("customers").get<std::vector<json>>();
  for (auto& customer_json : customers_json) {
    CustomerInfo customer;
    customer_json.at("user_id").get_to(customer.user_id_);
    customer_json.at("password").get_to(customer.password_);
    customer_json.at("balance").get_to(customer.balance_);
    customers_[customer.user_id_] = customer;
  }
}

void Shop::SaveData(const std::string& shop_id) {
  std::string shop_filepath = "../data/shop_" + shop_id + ".json";
  std::ofstream shop_file(shop_filepath);
  shop_file << shop_json_.dump(4);
  shop_file.close();
}

const std::unordered_map<std::string, shopping::Item>& Shop::GetItems() const {
  return items_;
}

const std::unordered_map<std::string, CustomerInfo>& Shop::GetCustomers()
    const {
  return customers_;
}

bool Shop::SetCustomer(const CustomerInfo& customer) {
  customers_[customer.user_id_] = customer;
  auto customers_json = shop_json_.at("customers").get<std::vector<json>>();
  bool add_flag = true;
  for (auto& customer_json : customers_json) {
    if (customer_json.at("user_id").get<std::string>() == customer.user_id_) {
      customer_json = {{"user_id", customer.user_id_},
                       {"password", customer.password_},
                       {"balance", customer.balance_}};
      add_flag = false;
      break;
    }
  }
  if (add_flag) {
    customers_json.push_back({{"user_id", customer.user_id_},
                              {"password", customer.password_},
                              {"balance", customer.balance_}});
  }
  shop_json_["customers"] = json(customers_json);
  return add_flag;
}

bool Shop::SetItem(const shopping::Item& item) {
  items_[item.item_id()] = item;
  auto items_json = shop_json_.at("items").get<std::vector<json>>();
  bool add_flag = true;
  for (auto& item_json : items_json) {
    if (item_json.at("item_id").get<std::string>() == item.item_id()) {
      item_json = {{"item_id", item.item_id()},
                   {"item_name", item.item_name()},
                   {"price", item.price()},
                   {"count", item.count()}};
      add_flag = false;
      break;
    }
  }
  if (add_flag) {
    items_json.push_back({{"item_id", item.item_id()},
                          {"item_name", item.item_name()},
                          {"price", item.price()},
                          {"count", item.count()}});
  }
  shop_json_["items"] = json(items_json);
  return add_flag;
}

void Shop::DeleteItem(const std::string& item_id) {
  items_.erase(item_id);
  auto items_json = shop_json_.at("items").get<std::vector<json>>();
  for (auto it = items_json.begin(); it != items_json.end(); ++it) {
    if (it->at("item_id").get<std::string>() == item_id) {
      items_json.erase(it);
      break;
    }
  }
  shop_json_["items"] = json(items_json);
}