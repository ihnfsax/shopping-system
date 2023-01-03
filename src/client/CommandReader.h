#ifndef CLIENT_COMMANDREADER_H_
#define CLIENT_COMMANDREADER_H_
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string>
#include <typeinfo>

class CommandReader {
 public:
  template <typename T>
  static bool ReadLine(T& res) {
    std::string input_line;
    std::getline(std::cin, input_line);
    boost::trim(input_line);
    T temp;
    try {
      temp = boost::lexical_cast<T>(input_line);
    } catch (const boost::bad_lexical_cast& e) {
      return true;
    }
    res = temp;
    return false;
  }
};

#endif  // CLIENT_COMMANDREADER_H_