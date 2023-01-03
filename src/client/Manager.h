#ifndef MANAGER_STORE_H_
#define MANAGER_STORE_H_

#include <iostream>

#include "Role.h"

class Manager : public Role {
 public:
  // Store() = default;
  // Store(Store&) = delete;
  // Store(Store&&) = delete;
  // Store& operator=(Store const&) = delete;
  // Store& operator=(Store&&) = delete;
  // ~Store() override = default;
  using Role::LoginOrRegister;
  using Role::Start;
  void Login() override;
  void MainMenu() override;
};

#endif  // MANAGER_STORE_H_