#include "Customer.h"

#include <grpcpp/client_context.h>
#include <grpcpp/support/status.h>

#include "CommandReader.h"
#include "src/protos/online_shopping.pb.h"

void Customer::Login() {
  std::string prompt_message =
      "\033[1;32mLog in as a customer...\033[0m\nPlease enter your user id: ";
  std::string user_id = CommandReader::ReadString(prompt_message);
  prompt_message = "Please enter your password: ";
  std::string password = CommandReader::ReadString(prompt_message);

  // RPC
  grpc::ClientContext context;
  shopping::LoginRequest request;
  request.set_user_type(shopping::UserType::CUSTOMER);
  request.set_user_id(user_id);
  request.set_password(password);
  request.set_shop_id(shop_id_);
  shopping::LoginReply reply;
  grpc::Status status =
      clients_[curr_srv_idx_].stub()->RPCLogin(&context, request, &reply);

  // Connection failed
  if (!status.ok()) {
    if (status.error_code() == grpc::StatusCode::UNAVAILABLE) {
      std::cout << "\033[1;31mServer " << clients_[curr_srv_idx_].shop_id()
                << "(" << clients_[curr_srv_idx_].server_address()
                << ") is unavailable.\033[0m" << std::endl;
      if (TryOtherServers()) {
        std::cout << "\033[1;31mAll servers is unavailable, please try again "
                     "later.\033[0m"
                  << std::endl;
      } else {
        std::cout << "\033[1;32mServer " << clients_[curr_srv_idx_].shop_id()
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
      balance_ = reply.balance();
      std::cout << "\033[1;32mLogin successfully! Your balance is " << balance_
                << ".\033[0m" << std::endl;
      MainMenu();
      break;
    case shopping::ErrorType::SHOP_NOT_FOUND:
      std::cout << "\033[1;31mShop " << shop_id_
                << " not found! Try to load shop...\033[0m" << std::endl;
      LoadShopInternal();
      LoginOrRegister();
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

void Customer::Register() {
  std::string prompt_message =
      "\033[1;32mRegister as a new customer...\033[0m\nPlease enter your user "
      "id: ";
  std::string user_id = CommandReader::ReadString(prompt_message);
  prompt_message = "Please enter your password: ";
  std::string password = CommandReader::ReadString(prompt_message);

  // RPC
  grpc::ClientContext context;
  shopping::RegisterRequest request;
  request.set_user_type(shopping::UserType::CUSTOMER);
  request.set_user_id(user_id);
  request.set_password(password);
  request.set_shop_id(shop_id_);
  shopping::RegisterReply reply;
  grpc::Status status =
      clients_[curr_srv_idx_].stub()->RPCRegister(&context, request, &reply);

  // Connection failed
  if (!status.ok()) {
    if (status.error_code() == grpc::StatusCode::UNAVAILABLE) {
      std::cout << "\033[1;31mServer " << clients_[curr_srv_idx_].shop_id()
                << "(" << clients_[curr_srv_idx_].server_address()
                << ") is unavailable.\033[0m" << std::endl;
      if (TryOtherServers()) {
        std::cout << "\033[1;31mAll servers is unavailable, please try again "
                     "later.\033[0m"
                  << std::endl;
      } else {
        std::cout << "\033[1;32mServer " << clients_[curr_srv_idx_].shop_id()
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
      balance_ = reply.balance();
      std::cout << "\033[1;32mRegister successfully! Login as " << user_id_
                << ". Your balance is " << balance_ << ".\033[0m" << std::endl;
      MainMenu();
      break;
    case shopping::ErrorType::SHOP_NOT_FOUND:
      std::cout << "\033[1;31mShop " << shop_id_
                << " not found! Try to load shop...\033[0m" << std::endl;
      LoadShopInternal();
      LoginOrRegister();
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

void Customer::MainMenu() {
  std::string prompt_message =
      "Welcome to the main menu!\nWhich function you'd like to perform?\n(1: "
      "List all items, 2: Buy something, 3: Add balance 4: Logout) ";
  size_t choice = CommandReader::ReadOption(prompt_message, 1, 4);

  switch (choice) {
    case 1:
      ListItems();
      break;
    case 2:
      Purchase();
      break;
    case 3:
      MoreBalance();
      break;
    case 4:
      std::cout << "\033[1;32mLogout...\033[0m" << std::endl;
      LoginOrRegister();
      break;
    default:
      MainMenu();
  }
}

void Customer::ListItems() {
  bool ret = User::ListItemsInternal();
  if (!ret) {
    std::cout << "Your balance is \033[1;32m" << balance_ << "\033[0m."
              << std::endl;
  }
  MainMenu();
}

void Customer::Purchase() {
  std::string prompt_message = "Please enter the item id: ";
  std::string item_id = CommandReader::ReadString(prompt_message);
  prompt_message = "Please enter the quantity: ";
  size_t buy_count = CommandReader::ReadOption(prompt_message, 0);

  if (buy_count == 0) {
    std::cout << "\033[1;31mInvalid quantity. Please try again.\033[0m"
              << std::endl;
    MainMenu();
    return;
  }

  // RPC
  grpc::ClientContext context;
  shopping::PurchaseRequest request;
  request.set_shop_id(shop_id_);
  request.set_user_id(user_id_);
  request.set_item_id(item_id);
  request.set_count(buy_count);
  shopping::PurchaseReply reply;
  grpc::Status status =
      clients_[curr_srv_idx_].stub()->RPCPurchase(&context, request, &reply);

  // Connection failed
  if (!status.ok()) {
    if (status.error_code() == grpc::StatusCode::UNAVAILABLE) {
      std::cout << "\033[1;31mServer " << clients_[curr_srv_idx_].shop_id()
                << "(" << clients_[curr_srv_idx_].server_address()
                << ") is unavailable.\033[0m" << std::endl;
      if (TryOtherServers()) {
        std::cout << "\033[1;31mAll servers is unavailable, please try again "
                     "later.\033[0m"
                  << std::endl;
      } else {
        std::cout << "\033[1;32mServer " << clients_[curr_srv_idx_].shop_id()
                  << "(" << clients_[curr_srv_idx_].server_address()
                  << ") is available, please try again.\033[0m" << std::endl;
      }
    } else {
      std::cout << "\033[1;31mPurchase failed, please try again.\033[0m"
                << std::endl;
    }
    MainMenu();
    return;
  }

  // Handle message
  switch (reply.error()) {
    case shopping::ErrorType::NO_ERROR:
      balance_ = reply.balance();
      std::cout << "\033[1;32mYou buy " << reply.item().item_name() << "("
                << buy_count << ") successfully! Your balance is " << balance_
                << " after purchasing.\033[0m" << std::endl;
      MainMenu();
      break;
    case shopping::ErrorType::SHOP_NOT_FOUND:
      std::cout << "\033[1;31mShop " << shop_id_
                << " not found! Try to load shop...\033[0m" << std::endl;
      LoadShopInternal();
      MainMenu();
      break;
    case shopping::ErrorType::USER_NOT_FOUND:
      std::cout << "\033[1;31mUser " << user_id_ << " not found! Please login "
                << "again.\033[0m" << std::endl;
      MainMenu();
      break;
    case shopping::ErrorType::ITEM_NOT_FOUND:
      std::cout << "\033[1;31mItem " << item_id << " not found! Please try "
                << "again.\033[0m" << std::endl;
      MainMenu();
      break;
    case shopping::ErrorType::ITEM_NOT_ENOUGH:
      std::cout << "\033[1;31mItem " << item_id
                << " doesn't have enough stock! Please try "
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

void Customer::MoreBalance() {
  std::string prompt_message = "Please enter the amount you want to add: ";
  double balance_to_add = CommandReader::ReadDouble(prompt_message);

  if (balance_to_add <= 0) {
    std::cout << "\033[1;31mInvalid quantity. Please try again.\033[0m"
              << std::endl;
    MainMenu();
    return;
  }

  // RPC
  grpc::ClientContext context;
  shopping::MoreBalanceRequest request;
  request.set_shop_id(shop_id_);
  request.set_user_id(user_id_);
  request.set_more_balance(balance_to_add);
  shopping::MoreBalanceReply reply;
  grpc::Status status =
      clients_[curr_srv_idx_].stub()->RPCMoreBalance(&context, request, &reply);

  // Connection failed
  if (!status.ok()) {
    if (status.error_code() == grpc::StatusCode::UNAVAILABLE) {
      std::cout << "\033[1;31mServer " << clients_[curr_srv_idx_].shop_id()
                << "(" << clients_[curr_srv_idx_].server_address()
                << ") is unavailable.\033[0m" << std::endl;
      if (TryOtherServers()) {
        std::cout << "\033[1;31mAll servers is unavailable, please try again "
                     "later.\033[0m"
                  << std::endl;
      } else {
        std::cout << "\033[1;32mServer " << clients_[curr_srv_idx_].shop_id()
                  << "(" << clients_[curr_srv_idx_].server_address()
                  << ") is available, please try again.\033[0m" << std::endl;
      }
    } else {
      std::cout << "\033[1;31mAdd balance failed, please try again.\033[0m"
                << std::endl;
    }
    MainMenu();
    return;
  }

  // Handle message
  switch (reply.error()) {
    case shopping::ErrorType::NO_ERROR:
      balance_ = reply.balance();
      std::cout << "\033[1;32mYou add " << balance_to_add
                << " credits successfully! Your balance is " << balance_
                << " now.\033[0m" << std::endl;
      MainMenu();
      break;
    case shopping::ErrorType::SHOP_NOT_FOUND:
      std::cout << "\033[1;31mShop " << shop_id_
                << " not found! Try to load shop...\033[0m" << std::endl;
      LoadShopInternal();
      MainMenu();
      break;
    case shopping::ErrorType::USER_NOT_FOUND:
      std::cout << "\033[1;31mUser " << user_id_ << " not found! Please login "
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