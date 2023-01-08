#include "Manager.h"

#include <grpcpp/client_context.h>
#include <grpcpp/support/status.h>

#include "CommandReader.h"
#include "src/protos/online_shopping.pb.h"

void Manager::Login() {
  std::string prompt_message =
      "\033[1;32mLog in as a shop manager...\033[0m\nPlease enter your user "
      "id: ";
  std::string user_id = CommandReader::ReadString(prompt_message);
  prompt_message = "Please enter your password: ";
  std::string password = CommandReader::ReadString(prompt_message);

  // RPC
  grpc::ClientContext context;
  shopping::LoginRequest request;
  request.set_user_type(shopping::UserType::MANAGER);
  request.set_user_id(user_id);
  request.set_password(password);
  shopping::LoginReply reply;
  grpc::Status status =
      clients_[curr_srv_idx_].stub()->RPCLogin(&context, request, &reply);

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
      std::cout << "\033[1;31mLogin failed, please try again.\033[0m"
                << std::endl;
    }
    LoginOrRegister();
    return;
  }

  // Handle Message
  switch (reply.error()) {
    case shopping::ErrorType::NO_ERROR:
      user_id_ = user_id;
      std::cout << "\033[1;32mLogin successfully!\033[0m" << std::endl;
      MainMenu();
      break;
    case shopping::ErrorType::USER_NOT_FOUND:
      std::cout << "\033[1;31mUser id " << user_id
                << " not found. Please try again.\033[0m" << std::endl;
      LoginOrRegister();
      break;
    case shopping::ErrorType::WRONG_PASSWORD:
      std::cout << "\033[1;31mWrong password. Please try again.\033[0m"
                << std::endl;
      LoginOrRegister();
      break;
    default:
      std::cout << "\033[1;31mUnknown error. Please try again.\033[0m"
                << std::endl;
      LoginOrRegister();
      break;
  }
}

void Manager::Register() {
  std::string prompt_message =
      "\033[1;32mRegister as a new manager...\033[0m\nPlease enter your user "
      "id: ";
  std::string user_id = CommandReader::ReadString(prompt_message);
  prompt_message = "Please enter your password: ";
  std::string password = CommandReader::ReadString(prompt_message);

  // RPC
  grpc::ClientContext context;
  shopping::RegisterRequest request;
  request.set_user_type(shopping::UserType::MANAGER);
  request.set_user_id(user_id);
  request.set_password(password);
  shopping::RegisterReply reply;
  grpc::Status status =
      clients_[curr_srv_idx_].stub()->RPCRegister(&context, request, &reply);

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
      std::cout << "\033[1;31mRegister failed, please try again.\033[0m"
                << std::endl;
    }
    LoginOrRegister();
    return;
  }

  // Handle Message
  switch (reply.error()) {
    case shopping::ErrorType::NO_ERROR:
      user_id_ = user_id;
      std::cout << "\033[1;32mRegister successfully! Login as " << user_id_
                << ".\033[0m" << std::endl;
      MainMenu();
      break;
    case shopping::ErrorType::USER_ALREADY_EXISTS:
      std::cout << "\033[1;31mUser id " << user_id
                << " already exists. Please try again.\033[0m" << std::endl;
      LoginOrRegister();
      break;
    default:
      std::cout << "\033[1;31mUnknown error. Please try again.\033[0m"
                << std::endl;
      LoginOrRegister();
      break;
  }
}

void Manager::MainMenu() {
  std::string prompt_message =
      "Welcome to the main menu!\nWhich function you'd like to perform?\n(1: "
      "List all items, 2: Edit or add one product, 3: Delete one product, 4: "
      "Logout) ";
  size_t choice = CommandReader::ReadUint(prompt_message, 1, 4);

  switch (choice) {
    case 1:
      ListItems();
      break;
    case 2:
      EditOrAddItem();
      break;
    case 3:
      DeleteItem();
      break;
    case 4:
      std::cout << "\033[1;32mLogout...\033[0m" << std::endl;
      LoginOrRegister();
      break;
    default:
      MainMenu();
  }
}

void Manager::ListItems() {
  User::ListItemsInternal();
  MainMenu();
}

void Manager::EditOrAddItem() {
  std::string prompt_message = "Please enter the item id: ";
  std::string item_id = CommandReader::ReadString(prompt_message);
  prompt_message = "Please enter the item name: ";
  std::string item_name = CommandReader::ReadString(prompt_message);
  prompt_message = "Please enter the item price: ";
  double price = CommandReader::ReadDouble(prompt_message);
  prompt_message = "Please enter the item count: ";
  uint32_t count = CommandReader::ReadUint(prompt_message);

  // RPC
  grpc::ClientContext context;
  shopping::EditOrAddItemRequest request;
  request.set_shop_id(shop_id_);
  request.mutable_item()->set_item_id(item_id);
  request.mutable_item()->set_item_name(item_name);
  request.mutable_item()->set_price(price);
  request.mutable_item()->set_count(count);
  shopping::EditOrAddItemReply reply;
  grpc::Status status = clients_[curr_srv_idx_].stub()->RPCEditOrAddItem(
      &context, request, &reply);

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
      std::cout << "\033[1;31mEdit or add item failed, please try again.\033[0m"
                << std::endl;
    }
    MainMenu();
    return;
  }

  // Handle Message
  switch (reply.error()) {
    case shopping::ErrorType::NO_ERROR:
      if (reply.is_added()) {
        std::cout << "\033[1;32mAdd item " << item_id << " successfully!\033[0m"
                  << std::endl;
      } else {
        std::cout << "\033[1;32mEdit item " << item_id
                  << " successfully!\033[0m" << std::endl;
      }
      MainMenu();
      break;
    case shopping::ErrorType::SHOP_NOT_FOUND:
      std::cout << "\033[1;31mShop " << shop_id_
                << " not found! Try to load shop...\033[0m" << std::endl;
      LoadShopInternal();
      MainMenu();
      break;
    default:
      std::cout << "\033[1;31mUnknown error. Please try again.\033[0m"
                << std::endl;
      MainMenu();
      break;
  }
}

void Manager::DeleteItem() {
  std::string prompt_message = "Please enter the item id: ";
  std::string item_id = CommandReader::ReadString(prompt_message);

  // RPC
  grpc::ClientContext context;
  shopping::DeleteItemRequest request;
  request.set_shop_id(shop_id_);
  request.set_item_id(item_id);
  shopping::DeleteItemReply reply;
  grpc::Status status =
      clients_[curr_srv_idx_].stub()->RPCDeleteItem(&context, request, &reply);

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
      std::cout << "\033[1;31mDelete item failed, please try again.\033[0m"
                << std::endl;
    }
    MainMenu();
    return;
  }

  // Handle Message
  switch (reply.error()) {
    case shopping::ErrorType::NO_ERROR:
      std::cout << "\033[1;32mDelete item " << item_id
                << " successfully!\033[0m" << std::endl;
      MainMenu();
      break;
    case shopping::ErrorType::SHOP_NOT_FOUND:
      std::cout << "\033[1;31mShop " << shop_id_
                << " not found! Try to load shop...\033[0m" << std::endl;
      LoadShopInternal();
      MainMenu();
      break;
    case shopping::ErrorType::ITEM_NOT_FOUND:
      std::cout << "\033[1;31mItem " << item_id << " not found! Please try "
                << "again.\033[0m" << std::endl;
      MainMenu();
      break;
    default:
      std::cout << "\033[1;31mUnknown error. Please try again.\033[0m"
                << std::endl;
      MainMenu();
      break;
  }
}