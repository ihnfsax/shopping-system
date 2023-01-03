#include "Shop.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

void Shop::LoadData(const std::string& shop_name) {
  std::string shop_filepath = "../data/shop_" + shop_name + ".json";
  std::ifstream shop_file(shop_filepath);
  json shop_json = json::parse(shop_file);
  shop_file.close();

  // load items from json file
  items_.clear();
  auto items_json = shop_json.at("items").get<std::vector<json>>();
  for (auto& item_json : items_json) {
    shopping::Item item;
    item.set_id(item_json.at("id").get<std::string>());
    item.set_name(item_json.at("name").get<std::string>());
    item.set_price(item_json.at("price").get<double>());
    item.set_count(item_json.at("count").get<uint32_t>());
    items_[item.id()] = item;
  }

  // load customer fron json file
  customers_.clear();
  auto customers_json = shop_json.at("customers").get<std::vector<json>>();
  for (auto& customer_json : customers_json) {
    shopping::User customer;
    customer.set_type(shopping::RoleType::CUSTOMER);
    customer.set_id(customer_json.at("account").get<std::string>());
    customer.set_password(customer_json.at("password").get<std::string>());
    customer.set_shop_id(shop_name);
    customer.set_balance(customer_json.at("balance").get<double>());
    customers_[customer.id()] = customer;
  }
}

const std::unordered_map<std::string, shopping::Item>& Shop::GetItems() const {
  return items_;
}