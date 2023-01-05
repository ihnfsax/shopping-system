#ifndef SERVER_RPCSERVER_H_
#define SERVER_RPCSERVER_H_

#include <grpcpp/server_context.h>
#include <grpcpp/support/status.h>

#include <nlohmann/json.hpp>
#include <unordered_map>
#include <vector>

#include "Shop.h"
#include "src/protos/online_shopping.grpc.pb.h"

class RPCServer final : public shopping::OnlineShopping::Service {
 public:
  explicit RPCServer(const std::string& shop_id,
                     const std::string& server_address);

  grpc::Status RPCLogin(grpc::ServerContext* context,
                        const shopping::LoginRequest* request,
                        shopping::LoginReply* reply) override;

  grpc::Status RPCRegister(grpc::ServerContext* context,
                           const shopping::RegisterRequest* request,
                           shopping::RegisterReply* reply) override;

  grpc::Status RPCListItems(grpc::ServerContext* context,
                            const shopping::ListItemsRequest* request,
                            shopping::ListItemsReply* reply) override;

  grpc::Status RPCPurchase(grpc::ServerContext* context,
                           const shopping::PurchaseRequest* request,
                           shopping::PurchaseReply* reply) override;

  grpc::Status RPCEditOrAddItem(grpc::ServerContext* context,
                                const shopping::EditOrAddItemRequest* request,
                                shopping::EditOrAddItemReply* reply) override;

  grpc::Status RPCDeleteItem(grpc::ServerContext* context,
                             const shopping::DeleteItemRequest* request,
                             shopping::DeleteItemReply* reply) override;

  grpc::Status RPCMoreBalance(grpc::ServerContext* context,
                              const shopping::MoreBalanceRequest* request,
                              shopping::MoreBalanceReply* reply) override;

  grpc::Status RPCLoadShop(grpc::ServerContext* context,
                           const shopping::LoadShopRequest* request,
                           shopping::LoadShopReply* reply) override;

  grpc::Status RPCCheckStatus(grpc::ServerContext* context,
                              const shopping::CheckStatusRequest* request,
                              shopping::CheckStatusReply* reply) override;

 private:
  void SaveManagers();
  bool TestServer(const std::string& server_address,
                  std::vector<std::string>& shop_ids);
  std::string server_address_;
  nlohmann::json managers_json_;
  std::unordered_map<std::string, Shop> shops_;
  std::unordered_map<std::string, ManagerInfo> managers_;
};

#endif  // SERVER_RPCSERVER_H_