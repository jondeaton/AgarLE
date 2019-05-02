#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>

#include <iostream>

#include "envs/ScreenEnvironment.hpp"
#include "envs/FullEnvironment.hpp"

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

  typedef agario::env::ScreenEnvironment<true, Width, Height> ScreenEnvironment;
  typedef agario::env::full::Environment FullEnvironment;

  pybind11::class_<ScreenEnvironment>(module, "ScreenEnvironment")
    .def(pybind11::init<int>())
    .def("step", &ScreenEnvironment::step)
    .def("get_state", [](const ScreenEnvironment &env) {

      auto &observation = env.get_state();

      auto data = (void *) observation.frame_data();
      std::vector<int> shape = {NumFrames, Width, Height, PIXEL_SIZE};
      std::vector<int> strides = {Width * Height * PIXEL_SIZE, Height * PIXEL_SIZE, PIXEL_SIZE, 1};

      auto arr = py::array_t<std::uint8_t>(py::buffer_info(data, sizeof(std::uint8_t),
                                                           py::format_descriptor<std::uint8_t>::format(),
                                                           observation.num_frames(),
                                                           shape, strides));
      return arr;
    })
    .def("done", &ScreenEnvironment::done)
    .def("take_action", &ScreenEnvironment::take_action, "x"_a, "y"_a, "act"_a)
    .def("reset", &ScreenEnvironment::reset);
  
  pybind11::class_<FullEnvironment>(module, "FullEnvironment")
    .def(pybind11::init<int>())
    .def("step", &FullEnvironment::step)
    .def("get_state", [](const FullEnvironment &env) {
      auto &observation = env.get_state();

      py::list data_list;

      const std::vector<float *> data_vec = observation.data();

      for (auto data_entry : data_vec) {
//         auto arr = py::array_t<std::uint8_t>(py::buffer_info());

//         data_list.append(arr);
      }


    })
    .def("done", &ScreenEnvironment::done)
    .def("take_action", &ScreenEnvironment::take_action, "x"_a, "y"_a, "act"_a)
    .def("reset", &ScreenEnvironment::reset);

  }

}
