#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>

#include <iostream>

#include "envs/ScreenEnvironment.hpp"

#define PIXEL_SIZE 3

namespace py = pybind11;

PYBIND11_MODULE(agario_screen_env, module) {
  using namespace pybind11::literals;
  module.doc() = "Agario Learning Environment (Screen)";

  constexpr unsigned Width = 256;
  constexpr unsigned Height = 256;
  constexpr unsigned NumFrames = 4;
  constexpr unsigned observation_size = NumFrames * Width * Height * PIXEL_SIZE;

  typedef agario::env::screen::ScreenEnvironment<true> ScreenEnvironment;
  pybind11::class_<ScreenEnvironment>(module, "Environment")
    .def(pybind11::init<unsigned, unsigned, unsigned>())
    .def("step", &ScreenEnvironment::step)
    .def("get_state", [](const ScreenEnvironment &env) {

      auto &observation = env.get_state();
      auto data = (void *) observation.frame_data();
      std::vector<int> shape = {NumFrames, Width, Height, PIXEL_SIZE};
      std::vector<int> strides = {Width * Height * PIXEL_SIZE * sizeof(std::uint8_t),
                                  Height * PIXEL_SIZE, PIXEL_SIZE * sizeof(std::uint8_t),
                                  1 * sizeof(std::uint8_t)};

      auto format = py::format_descriptor<std::uint8_t>::format();
      auto buffer = py::buffer_info(data, sizeof(std::uint8_t), format, shape.size(), shape, strides);
      auto arr = py::array_t<std::uint8_t>(buffer);
      return arr;
    })
    .def("done", &ScreenEnvironment::done)
    .def("take_action", &ScreenEnvironment::take_action, "x"_a, "y"_a, "act"_a)
    .def("reset", &ScreenEnvironment::reset);

}

