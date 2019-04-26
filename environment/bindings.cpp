#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

int add(int x, int y) { return x + y; }

class Adder {
public:
  Adder(int x) : x_(x){};
  int add(int y) { return x_ + y; }

  void setAddition(int x) { x_ = x; }
  int getAddition() { return x_; }

private:
  int x_;
};

std::string join(std::vector<std::string> tojoin) {
  std::string ret;
  for (auto c : tojoin) {
    ret += c;
  }
  return ret;
}


PYBIND11_MODULE(agario_env, mymodule) {
  using namespace pybind11::literals; // for _a literal to define arguments
  mymodule.doc() = "example module to export code";

  mymodule.def("add", &add, "Add 2 numbers together", "x"_a, "y"_a);

  pybind11::class_<Adder>(mymodule, "Adder")
    .def(pybind11::init<int>())
    .def("add", &Adder::add)
    .def_property("addition", &Adder::getAddition, &Adder::setAddition);


  mymodule.def("join", &join, "Join a list into a string", "tojoin"_a);
}
