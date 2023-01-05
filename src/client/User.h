#ifndef CLIENT_USER_H_
#define CLIENT_USER_H_

#include <grpcpp/channel.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "src/protos/online_shopping.grpc.pb.h"

struct ClientInfo {
 public:
  ClientInfo(const std::string& server_id, const std::string& server_address,
             std::shared_ptr<grpc::Channel> channel)
      : server_id_(server_id),
        server_address_(server_address),
        stub_(shopping::OnlineShopping::NewStub(channel)) {}
  const std::string& server_id() const { return server_id_; }
  const std::string& server_address() const { return server_address_; }
  const std::unique_ptr<shopping::OnlineShopping::Stub>& stub() const {
    return stub_;
  }

 private:
  std::string server_id_;
  std::string server_address_;
  std::unique_ptr<shopping::OnlineShopping::Stub> stub_;
};

class User {
 public:
  User();
  User(User&) = delete;
  User(User&&) = delete;
  User& operator=(User const&) = delete;
  User& operator=(User&&) = delete;
  virtual void Start();
  virtual void LoginOrRegister();
  virtual void Login() = 0;
  virtual void Register() = 0;
  virtual void MainMenu() = 0;
  virtual void ListItems() = 0;
  virtual ~User(){};

 protected:
  // Return true if list fails
  bool ListItemsInternal();
  // Return true if load shop fails
  bool LoadShopInternal();
  // Return true if no server is available
  bool TryOtherServers();
  std::string shop_id_;
  std::string user_id_;
  std::vector<ClientInfo> clients_;
  std::vector<ClientInfo>::size_type curr_srv_idx_;
};

#endif  // CLIENT_USER_H_