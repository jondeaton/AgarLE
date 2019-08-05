#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>

#include <iostream>
#include <environment/envs/FullEnvironment.hpp>
#include <environment/envs/GridEnvironment.hpp>
#include <environment/envs/RamEnvironment.hpp>

#ifdef INCLUDE_SCREEN_ENV
#include <environment/envs/ScreenEnvironment.hpp>
#endif

static constexpr bool renderable =
#ifdef RENDERABLE
 true
#else
  false
#endif
  ;

namespace py = pybind11;

PYBIND11_MODULE(agarle, module) {
  using namespace pybind11::literals;
  module.doc() = "Agar.io Learning Environment";

  /* ================ Full Environment ================ */
  using FullEnvironment = agario::env::FullEnvironment<renderable>;

  pybind11::class_<FullEnvironment>(module, "FullEnvironment")
    .def(pybind11::init<int, int, bool, int, int, int>())
    .def("step", &FullEnvironment::step)
    .def("get_state", [](const FullEnvironment &env) {
      auto &observation = env.get_state();

      // return type is list of numpy arrays
      py::list data_list;

      const auto &data_vec = observation.data();
      const auto &shapes = observation.shapes();

      /**
       * for each data buffer stored in the observation
       * make a numpy array out of it and append it to the
       * list of numpy arrays
       */
      for (int i = 0; i < data_vec.size(); i++) {
        auto *data = const_cast<float*>(data_vec[i]);

        std::vector<int> shape = shapes[i];
        std::vector<int> strides = {(int) (shape[1] * sizeof(float)),
                                    sizeof(float)};

        auto format = py::format_descriptor<float>::format();
        auto buffer = py::buffer_info(data, sizeof(float), format, shape.size(), shape, strides);
        auto arr = py::array_t<float>(buffer);
        data_list.append(arr);
      }

      return data_list;
    })
    .def("done", &FullEnvironment::done)
    .def("take_action", &FullEnvironment::take_action, "x"_a, "y"_a, "act"_a)
    .def("reset", &FullEnvironment::reset)
    .def("render", &FullEnvironment::render);


  /* ================ Grid Environment ================ */
  using GridEnvironment = agario::env::GridEnvironment<int, renderable>;

  pybind11::class_<GridEnvironment>(module, "GridEnvironment")
    .def(pybind11::init<int, int, bool, int, int, int>())
    .def("configure_observation", [](GridEnvironment &env, const py::dict &config) {

      int num_frames = config.contains("num_frames")      ? config["num_frames"].cast<int>() : 2;
      int grid_size  = config.contains("grid_size")       ? config["grid_size"].cast<int>() : DEFAULT_GRID_SIZE;
      bool cells     = config.contains("observe_cells")   ? config["observe_cells"].cast<bool>()   : true;
      bool others    = config.contains("observe_others")  ? config["observe_others"].cast<bool>()  : true;
      bool viruses   = config.contains("observe_viruses") ? config["observe_viruses"].cast<bool>() : true;
      bool pellets   = config.contains("observe_pellets") ? config["observe_pellets"].cast<bool>() : true;

      env.configure_observation(num_frames, grid_size, cells, others, viruses, pellets);
    })
    .def("observation_shape", [](const GridEnvironment &env) {
      const auto &shape = env.observation_shape();
      return py::make_tuple(shape[0], shape[1], shape[2]);
    })
    .def("step", &GridEnvironment::step)
    .def("get_state", [](const GridEnvironment &env) {
      using dtype = GridEnvironment::dtype;

      auto &observation = env.get_state();

      /// make a copy of the data for the numpy array to take ownership of
      auto *data = new dtype[observation.length()];
      std::copy(observation.data(), observation.data() + observation.length(), data);

      const auto &shape = observation.shape();
      const auto &strides = observation.strides();

      py::capsule cleanup(data, [](void *ptr) {
        auto *data_pointer = reinterpret_cast<dtype*>(ptr);
        delete[] data_pointer;
      });

      return py::array_t<dtype>(shape, strides, data, cleanup);
    })
    .def("done", &GridEnvironment::done)
    .def("take_action", &GridEnvironment::take_action, "x"_a, "y"_a, "act"_a)
    .def("reset", &GridEnvironment::reset)
    .def("render", &GridEnvironment::render);


  /* ================ Ram Environment ================ */
  using RamEnvironment = agario::env::RamEnvironment<renderable>;

  pybind11::class_<RamEnvironment>(module, "RamEnvironment")
    .def(pybind11::init<int, int, bool, int, int, int>())
    .def("observation_shape", [](const RamEnvironment &env) {
      return py::make_tuple(env.observation_length());
    })
    .def("step", &RamEnvironment::step)
    .def("get_state", [](const RamEnvironment &env) {
      using dtype = typename RamEnvironment::dtype;

      auto observation = env.get_state();

      auto *data = const_cast<dtype *>(observation.data());
      auto shape = observation.shape();
      auto strides = observation.strides();

      auto format = py::format_descriptor<dtype>::format();
      auto buffer = py::buffer_info(data, sizeof(dtype), format, shape.size(), shape, strides);
      auto arr = py::array_t<dtype>(buffer);
      return arr;
    })
    .def("done", &RamEnvironment::done)
    .def("take_action", &RamEnvironment::take_action, "x"_a, "y"_a, "act"_a)
    .def("reset", &RamEnvironment::reset)
    .def("render", &RamEnvironment::render);


  /* ================ Screen Environment ================ */
  /* we only include this conditionally if OpenGL was found available for linking */

#ifdef INCLUDE_SCREEN_ENV

  using ScreenEnvironment = agario::env::ScreenEnvironment<true>;

  pybind11::class_<ScreenEnvironment>(module, "ScreenEnvironment")
    .def(pybind11::init<int, int, bool, int, int, int, screen_len, screen_len>())
    .def("step", &ScreenEnvironment::step)
    .def("get_state", [](const ScreenEnvironment &env) {

      auto &observation = env.get_state();
      auto *data = const_cast<std::uint8_t *>(observation.frame_data());
      auto shape = observation.shape();
      auto strides = observation.strides();

      auto format = py::format_descriptor<std::uint8_t>::format();
      auto buffer = py::buffer_info(data, sizeof(std::uint8_t), format, shape.size(), shape, strides);
      auto arr = py::array_t<std::uint8_t>(buffer);
      return arr;
    })
    .def("done", &ScreenEnvironment::done)
    .def("take_action", &ScreenEnvironment::take_action, "x"_a, "y"_a, "act"_a)
    .def("reset", &ScreenEnvironment::reset)
    .def("render", &ScreenEnvironment::render);

#endif

}
