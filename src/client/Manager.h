#ifndef MANAGER_STORE_H_
#define MANAGER_STORE_H_

#include <iostream>

#include "User.h"

class Manager : public User {
 public:
  using User::LoginOrRegister;
  using User::Start;
  void Login() override;
  void Register() override;
  void MainMenu() override;
  void ListItems() override;
  void EditOrAddItem();
  void DeleteItem();
};

#endif  // MANAGER_STORE_H_