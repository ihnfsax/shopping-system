#include <iostream>
#include <memory>

#include "CommandReader.h"
#include "Customer.h"
#include "Manager.h"

void WelcomePage() {
  std::string prompt_message =
      "Welcome! Choose your user type: \n(1: Customer, 2: Manager) ";
  size_t user_type = CommandReader::ReadOption(prompt_message, 1, 2);

  std::shared_ptr<User> user;
  switch (user_type) {
    case 1:
      user = std::make_shared<Customer>();
      break;
    case 2:
      user = std::make_shared<Manager>();
      break;
    default:
      WelcomePage();
  }

  user->Start();
}

int main() {
  WelcomePage();
  return 0;
}