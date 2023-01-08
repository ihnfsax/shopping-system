#ifndef SERVER_SHOP_H_
#define SERVER_SHOP_H_
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

#include "src/protos/online_shopping.pb.h"

struct CustomerInfo {
  std::string user_id_;
  std::string password_;
  double balance_;
};

struct ManagerInfo {
  std::string user_id_;
  std::string password_;
};

class Shop {
 public:
  void LoadData(const std::string& shop_id);
  void SaveData(const std::string& shop_id);
  const std::unordered_map<std::string, shopping::Item>& GetItems() const;
  const std::unordered_map<std::string, CustomerInfo>& GetCustomers() const;
  // return true if item is added.
  bool SetItem(const shopping::Item& item);
  // return true if customer is added.
  bool SetCustomer(const CustomerInfo& customer);
  void DeleteItem(const std::string& item_id);

 private:
  nlohmann::json shop_json_;
  std::unordered_map<std::string, shopping::Item> items_;
  std::unordered_map<std::string, CustomerInfo> customers_;
};
#endif  // SERVER_SHOP_H_