#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>

#include <iostream>
#include "envs/FullEnvironment.hpp"

namespace py = pybind11;

PYBIND11_MODULE(agario_full_env, module) {
  using namespace pybind11::literals;module.
    doc() = "Agario Learning Environment (Full)";
  typedef agario::env::full::Environment<false> FullEnvironment;

  pybind11::class_<FullEnvironment>(module, "Environment")
    .def(pybind11::init<unsigned, unsigned, bool, unsigned, unsigned, unsigned>())
    .def("step", &FullEnvironment::step)
    .def("get_state", [](const FullEnvironment &env) {
      auto &observation = env.get_state();
      py::list data_list;

      const auto &data_vec = observation.data();
      const auto &shapes = observation.shapes();

      for (int i = 0; i < data_vec.size(); i++) {
        void *data = (void *) data_vec[i];
        std::vector<int> shape = shapes[i];
        std::vector<int> strides = {(int) (shape[1] * sizeof(float)),
                                    sizeof(float)};

        auto buffer = py::buffer_info(data, sizeof(float),
                                      py::format_descriptor<float>::format(),
                                      2, shape, strides);

        auto arr = py::array_t<float>(buffer);

        data_list.append(arr);
      }

      return
        data_list;
    })
    .def("done", &FullEnvironment::done)
    .def("take_action", &FullEnvironment::take_action, "x"_a, "y"_a, "act"_a)
    .def("reset", &FullEnvironment::reset)
    .def("render", &FullEnvironment::render);

}
