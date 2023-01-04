#ifndef SERVER_SHOP_H_
#define SERVER_SHOP_H_
#include <string>
#include <unordered_map>

#include "src/protos/online_shopping.pb.h"
class Shop {
 public:
  void LoadData(const std::string& shop_id);
  const std::unordered_map<std::string, shopping::Item>& GetItems() const;

 private:
  std::unordered_map<std::string, shopping::Item> items_;
  std::unordered_map<std::string, shopping::User> customers_;
};
#endif  // SERVER_SHOP_H_