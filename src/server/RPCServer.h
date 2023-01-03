#ifndef SERVER_RPCSERVER_H_
#define SERVER_RPCSERVER_H_

#include <grpcpp/server_context.h>
#include <grpcpp/support/status.h>

#include <unordered_map>

#include "Shop.h"
#include "online_shopping.grpc.pb.h"

class RPCServerImpl final : public shopping::OnlineShopping::Service {
 public:
  explicit RPCServerImpl(const std::string& server_name);
  grpc::Status GetItems(grpc::ServerContext* context,
                        const shopping::GetItemsRequest* request,
                        shopping::GetItemsReply* response) override;

 private:
  void LoadManagers();

 private:
  std::unordered_map<std::string, Shop> shops_;
  std::unordered_map<std::string, shopping::User> managers_;
};

#endif  // SERVER_RPCSERVER_H_