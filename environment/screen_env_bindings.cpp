#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>

#include <iostream>

#include "envs/ScreenEnvironment.hpp"

// there are three numbers in each pixel (RGB)
#define PIXEL_LEN 3

namespace py = pybind11;

PYBIND11_MODULE(agario_screen_env, module) {
  using namespace pybind11::literals;
  module.doc() = "Agario Learning Environment (Screen)";

  typedef agario::env::ScreenEnvironment<true> ScreenEnvironment;

  pybind11::class_<ScreenEnvironment>(module, "Environment")
    .def(pybind11::init<int, int, bool, int, int, int, screen_len, screen_len>())
    .def("step", &ScreenEnvironment::step)
    .def("get_state", [](const ScreenEnvironment &env) {

      auto &observation = env.get_state();
      auto data = (void *) observation.frame_data();
      auto shape = observation.shape();
      auto strides = observation.strides();

      auto format = py::format_descriptor<std::uint8_t>::format();
      auto buffer = py::buffer_info(data, sizeof(std::uint8_t), format, shape.size(), shape, strides);
      auto arr = py::array_t<std::uint8_t>(buffer);
      return arr;
    })
    .def("done", &ScreenEnvironment::done)
    .def("take_action", &ScreenEnvironment::take_action, "x"_a, "y"_a, "act"_a)
    .def("reset", &ScreenEnvironment::reset);

}

