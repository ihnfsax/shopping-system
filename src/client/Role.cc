#include "Role.h"

#include <iostream>

#include "CommandReader.h"

void Role::Start() {
  std::cout << "\n\n\n";
  std::cout << "Choose the server you want to connect to: \n";
  std::cout << "(1: S1, 2: S2) ";
  int server_number = 0;
  while (CommandReader::ReadLine<int>(server_number)) {
    Start();
  }
  switch (server_number) {
    case 1:
      std::cout << "Connected to Server 1!" << std::endl;
      break;
    case 2:
      std::cout << "Connected to Server 2!" << std::endl;
      break;
    default:
      Start();
  }
  LoginOrRegister();
}

void Role::LoginOrRegister() {
  std::cout << "\n\n\n";
  std::cout << "Do you want to login or register?\n";
  std::cout << "(1: login, 2: register) ";
  int login_or_register = 0;
  while (CommandReader::ReadLine<int>(login_or_register)) {
    LoginOrRegister();
  }
  switch (login_or_register) {
    case 1:
      Login();
      break;
    case 2:
      Register();
      break;
    default:
      LoginOrRegister();
  }
}