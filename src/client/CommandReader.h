#ifndef CLIENT_COMMANDREADER_H_
#define CLIENT_COMMANDREADER_H_

#include <cstddef>
#include <cstdint>
#include <string>

class CommandReader {
 public:
  static std::string ReadString(const std::string& prompt_message);

  static size_t ReadUint(const std::string& prompt_message,
                         size_t min_value = 0, size_t max_value = SIZE_MAX);

  static double ReadDouble(const std::string& prompt_message,
                           bool is_positive = true);
};

#endif  // CLIENT_COMMANDREADER_H_