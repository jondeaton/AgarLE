#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>

#include <iostream>
#include <environment.hpp>

void say_hello() { std::cout << "hello world!" << std::endl; }
int add(int x, int y) { return x + y; }

namespace py = pybind11;

PYBIND11_MODULE(agario_env, module) {
  using namespace pybind11::literals; // for _a literal to define arguments
  module.doc() = "Agario Learning Environment";

  module.def("say_hello", &say_hello, "say hello");
  module.def("add", &add, "Add 2 numbers together", "x"_a, "y"_a);

  constexpr unsigned Width = 256;
  constexpr unsigned Height = 256;
  constexpr unsigned NumFrames = 4;
  constexpr unsigned observation_size = NumFrames * Width * Height * 3;

  typedef agario::environment::Environment<true, Width, Height, NumFrames> Environment;

  pybind11::class_<Environment>(module, "Environment")
    .def(pybind11::init<int>())
    .def("step", &Environment::step)
    .def("get_state", [](const Environment &env) {
      auto observation = env.get_state();

      auto frame_buffer = new std::uint8_t[observation_size];
      std::memcpy(frame_buffer,
                  observation.frame_data,
                  observation_size * sizeof(std::uint8_t));

      py::capsule _free(frame_buffer, [](void *f) {
        auto foo = reinterpret_cast<std::uint8_t *>(f);
        delete[] foo;
      });

      std::vector<int> shape = {NumFrames, Width, Height, 3};
      std::vector<int> strides = {Width * Height * 3, Height * 3, 3, 1};

      return py::array_t<std::uint8_t>(shape, strides, frame_buffer, _free);
    })
    .def("done", &Environment::done)
    .def("take_action", &Environment::take_action, "x"_a, "y"_a, "act"_a)
    .def("reset", &Environment::reset);
}
