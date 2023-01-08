#ifndef CLIENT_CUSTOMER_H_
#define CLIENT_CUSTOMER_H_

#include <iostream>

#include "User.h"

class Customer : public User {
 public:
  using User::LoginOrRegister;
  using User::Start;
  void Login() override;
  void Register() override;
  void MainMenu() override;
  void ListItems() override;
  void Purchase();
  void MoreBalance();

 private:
  double balance_;
};

#endif  // CLIENT_CUSTOMER_H_