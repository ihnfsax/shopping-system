#ifndef CLIENT_CUSTOMER_H_
#define CLIENT_CUSTOMER_H_

#include <iostream>

#include "Role.h"

class Customer : public Role {
 public:
  using Role::LoginOrRegister;
  using Role::Start;
  void Login() override;
  void MainMenu() override;
};

#endif  // CLIENT_CUSTOMER_H_