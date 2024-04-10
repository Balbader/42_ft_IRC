#include <iostream>
#include <sstream>
#include <vector>
#include <string>

std::vector<std::string> split(const std::string &src, const std::string &sep) {
  
  std::string str = src;
  std::vector<std::string> split_str;
  std::string element;
  size_t pos = 0;
  while (( pos = str.find(sep)) != std::string::npos) {
    element = str.substr(0, pos);
    split_str.push_back(element);
    str.erase(0, pos + sep.length());
  }

  split_str.push_back(str);
  return (split_str);
}
