#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
int main() {
  std::string sa;
  std::getline(std::cin, sa);
  json j = sa;
  try {
    std::string sb = j.dump();
    std::cout << sb << '\n';
  }
  catch (json::type_error ece) {
    std::cerr << "invalid data in string\n";
  }
}
