#include "CommandReader.h"

#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

std::string CommandReader::ReadString(const std::string& prompt_message) {
  std::string input_line;
  do {
    std::cout << prompt_message;
    std::getline(std::cin, input_line);
    boost::trim(input_line);
    if (!input_line.empty()) {
      break;
    } else {
      std::cout << "\033[1;31mInvalid input!\033[0m\n" << std::endl;
    }
  } while (true);
  return input_line;
}

size_t CommandReader::ReadUint(const std::string& prompt_message,
                               size_t min_option, size_t max_option) {
  size_t ret = 0;
  do {
    std::cout << prompt_message << std::flush;
    std::string input_line;
    std::getline(std::cin, input_line);
    boost::trim(input_line);
    try {
      ret = boost::lexical_cast<size_t>(input_line);
      if (ret >= min_option && ret <= max_option) {
        break;
      } else {
        std::cout << "\033[1;31mInvalid input!\033[0m\n" << std::endl;
      }
    } catch (const boost::bad_lexical_cast& e) {
      std::cout << "\033[1;31mInvalid input!\033[0m\n" << std::endl;
    }
  } while (true);
  return ret;
}

double CommandReader::ReadDouble(const std::string& prompt_message,
                                 bool is_positive) {
  double ret = 0;
  do {
    std::cout << prompt_message << std::flush;
    std::string input_line;
    std::getline(std::cin, input_line);
    boost::trim(input_line);
    try {
      ret = boost::lexical_cast<double>(input_line);
      if (ret >= 0 || !is_positive) {
        break;
      } else {
        std::cout << "\033[1;31mInvalid input!\033[0m\n" << std::endl;
      }
    } catch (const boost::bad_lexical_cast& e) {
      std::cout << "\033[1;31mInvalid input!\033[0m\n" << std::endl;
    }
  } while (true);
  return ret;
}