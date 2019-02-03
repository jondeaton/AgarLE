#ifndef AGARIO_USER_HPP
#define AGARIO_USER_HPP

#include <string>

class User {
public:
  explicit User(std::string name) : name(std::move(name)) { }

private:
  std::string name;

};

#endif //AGARIO_USER_HPP
