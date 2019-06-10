#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>

#include <iostream>

#include "envs/GridEnvironment.hpp"

static constexpr bool renderable =
#ifdef RENDERABLE
  true
#else
  false
#endif
;

namespace py = pybind11;

PYBIND11_MODULE(agario_grid_env, module) {
  using namespace pybind11::literals;
  module.doc() = "Agario Learning Environment (Grid)";

  typedef agario::env::GridEnvironment<int, renderable> GridEnvironment;
  typedef GridEnvironment::dtype dtype;

  pybind11::class_<GridEnvironment>(module, "GridEnvironment")
    .def(pybind11::init<int, int, bool, int, int, int>())
    .def("configure_observation", [](GridEnvironment &env, py::dict config) {

      int grid_size = config.contains("grid_size") ? config["grid_size"].cast<int>() : DEFAULT_GRID_SIZE;
      bool cells    = config.contains("observe_cells")   ? config["observe_cells"].cast<bool>()   : true;
      bool others   = config.contains("observe_others")  ? config["observe_others"].cast<bool>()  : true;
      bool viruses  = config.contains("observe_viruses") ? config["observe_viruses"].cast<bool>() : true;
      bool pellets  = config.contains("observe_pellets") ? config["observe_pellets"].cast<bool>() : true;

      env.configure_observation(grid_size, cells, others, viruses, pellets);
    })
    .def("step", &GridEnvironment::step)
    .def("get_state", [](const GridEnvironment &env) {

      auto observation = env.get_state();

      auto data = (void *) observation.data();
      auto shape = observation.shape();
      auto strides = observation.strides();

      auto format = py::format_descriptor<dtype>::format();
      auto buffer = py::buffer_info(data, sizeof(dtype), format, shape.size(), shape, strides);
      auto arr = py::array_t<dtype>(buffer);
      return arr;
    })
    .def("done", &GridEnvironment::done)
    .def("take_action", &GridEnvironment::take_action, "x"_a, "y"_a, "act"_a)
    .def("reset", &GridEnvironment::reset)
    .def("render", &GridEnvironment::render);
}

