#include <iostream>
#include <memory>

#include "CommandReader.h"
#include "Customer.h"
#include "Manager.h"

void WelcomePage() {
  std::cout << "\n\n\n";
  std::cout << "Welcome! Chooose your role: \n";
  std::cout << "(1: Customer, 2: Manager) ";
  int role_type = 0;
  while (CommandReader::ReadLine<int>(role_type)) {
    WelcomePage();
  }
  std::shared_ptr<Role> role;
  switch (role_type) {
    case 1:
      role = std::make_shared<Customer>();
      break;
    case 2:
      role = std::make_shared<Manager>();
      break;
    default:
      WelcomePage();
  }

  role->Start();
}

int main() {
  WelcomePage();
  return 0;
}