#include "Manager.h"

#include "CommandReader.h"

void Manager::Login() {
  std::cout << "\n\n\n";
  std::cout << "Log in as a store manager...\n";
  std::cout << "Please enter your username: ";
  std::string username;
  while (CommandReader::ReadLine<std::string>(username)) {
    Login();
  }
  std::cout << "Please enter your password: ";
  std::string password;
  while (CommandReader::ReadLine<std::string>(password)) {
    Login();
  }
  // 网络通信
  std::cout << "Login successfully!" << std::endl;
}

void Manager::MainMenu() {
  std::cout << "\n\n\n";
  std::cout << "Welcome to the main menu!\n";
  std::cout << "Which function you'd like to perform?\n";
  std::cout << "(1: List all products, 2: Buy something, 3: Logout) ";
  int choice = 0;
  while (CommandReader::ReadLine<int>(choice)) {
    MainMenu();
  }
}