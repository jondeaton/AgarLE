#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>

#include <iostream>
#include <environment.hpp>

#define PIXEL_SIZE 3

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
  constexpr unsigned observation_size = NumFrames * Width * Height * PIXEL_SIZE;

  typedef agario::environment::Environment<true, Width, Height> Environment;

  pybind11::class_<Environment>(module, "Environment")
    .def(pybind11::init<int>())
    .def("step", &Environment::step)
    .def("get_state", [](const Environment &env) {

      auto &observation = env.get_state();
      auto data = new std::uint8_t[observation.size()];
      std::memcpy(data, observation.frame_data(), observation.size());

      std::vector<int> shape = {NumFrames, Width, Height, PIXEL_SIZE};
      std::vector<int> strides = {Width * Height * PIXEL_SIZE, Height * PIXEL_SIZE, PIXEL_SIZE, 1};

      auto arr = py::array_t<std::uint8_t>(py::buffer_info(data,
                                                           sizeof(std::uint8_t),
                                                           py::format_descriptor<std::uint8_t>::format(),
                                                           4, shape, strides));
      delete[] data;
      return arr;
    })
    .def("done", &Environment::done)
    .def("take_action", &Environment::take_action, "x"_a, "y"_a, "act"_a)
    .def("reset", &Environment::reset);
}
