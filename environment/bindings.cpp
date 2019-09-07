#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>

#include <tuple>
#include <iostream>
//#include <environment/envs/FullEnvironment.hpp>
#include <environment/envs/GridEnvironment.hpp>
#include <environment/envs/RamEnvironment.hpp>

#ifdef INCLUDE_SCREEN_ENV
//#include <environment/envs/ScreenEnvironment.hpp>
#endif

static constexpr bool renderable =
#ifdef RENDERABLE
  true
#else
  false
#endif
  ;

namespace py = pybind11;


template <class Tuple,
  class T = std::decay_t<std::tuple_element_t<0, std::decay_t<Tuple>>>>
std::vector<T> to_vector(Tuple&& tuple) {
  return std::apply([](auto&&... elems) {
    return std::vector<T>{std::forward<decltype(elems)>(elems)...};
  }, std::forward<Tuple>(tuple));
}

/* converts a python list of actions to the C++ action wrapper*/
std::vector<agario::env::Action> to_action_vector(const py::list &actions) {
  std::vector<agario::env::Action> acts;
  acts.reserve(actions.size());

  for (auto &action : actions) {
    auto t = py::cast<py::tuple>(action);

    auto dx = py::cast<float>(t[0]);
    auto dy = py::cast<float>(t[1]);
    auto a = static_cast<agario::action>(py::cast<int>(t[2]));

    acts.emplace_back(dx, dy, a);
  }
  return acts;
}

/* extracts observations from each agent, wrapping them in NumPy arrays */
template <typename Environment>
py::list get_state(const Environment &environment) {
  using dtype = typename Environment::dtype;

  auto &observations = environment.get_observations();
  py::list obs;
  for (auto &observation : observations) {
    // make a copy of the data for the numpy array to take ownership of
    auto *data = new dtype[observation.length()];
    std::copy(observation.data(), observation.data() + observation.length(), data);

    const auto &shape = observation.shape();
    const auto &strides = observation.strides();

    py::capsule cleanup(data, [](void *ptr) {
      auto *data_pointer = reinterpret_cast<dtype*>(ptr);
      delete[] data_pointer;
    });

    // add the numpy array to the list of observations
    obs.append(py::array_t<dtype>(to_vector(shape), to_vector(strides), data, cleanup));
  }
  return obs; // list of numpy arrays
}

PYBIND11_MODULE(agarle, module) {
  using namespace pybind11::literals;
  module.doc() = "Agar.io Learning Environment";

  /* ================ Grid Environment ================ */
  using GridEnvironment = agario::env::GridEnvironment<int, renderable>;

  pybind11::class_<GridEnvironment>(module, "GridEnvironment")
    .def(pybind11::init<int, int, int, bool, int, int, int>())
    .def("seed", &GridEnvironment::seed)
    .def("configure_observation", [](GridEnvironment &env, const py::dict &config) {

      int num_frames = config.contains("num_frames")      ? config["num_frames"].cast<int>() : 2;
      int grid_size  = config.contains("grid_size")       ? config["grid_size"].cast<int>() : DEFAULT_GRID_SIZE;
      bool cells     = config.contains("observe_cells")   ? config["observe_cells"].cast<bool>()   : true;
      bool others    = config.contains("observe_others")  ? config["observe_others"].cast<bool>()  : true;
      bool viruses   = config.contains("observe_viruses") ? config["observe_viruses"].cast<bool>() : true;
      bool pellets   = config.contains("observe_pellets") ? config["observe_pellets"].cast<bool>() : true;

      env.configure_observation(num_frames, grid_size, cells, others, viruses, pellets);
    })
    .def("observation_shape", &GridEnvironment::observation_shape)
    .def("dones", &GridEnvironment::dones)
    .def("take_actions", [](GridEnvironment &env, const py::list &actions) {
      env.take_actions(to_action_vector(actions));
    })
    .def("reset", &GridEnvironment::reset)
    .def("render", &GridEnvironment::render)
    .def("step", &GridEnvironment::step)
    .def("get_state", &get_state<GridEnvironment>);

  /* ================ Ram Environment ================ */
  using RamEnvironment = agario::env::RamEnvironment<renderable>;

  pybind11::class_<RamEnvironment>(module, "RamEnvironment")
    .def(pybind11::init<int, int, int, bool, int, int, int>())
    .def("seed", &RamEnvironment::seed)
    .def("observation_shape", &RamEnvironment::observation_shape)
    .def("dones", &RamEnvironment::dones)
    .def("take_actions", [](RamEnvironment &env, const py::list &actions) {
      env.take_actions(to_action_vector(actions));
    })
    .def("reset", &RamEnvironment::reset)
    .def("render", &RamEnvironment::render)
    .def("step", &RamEnvironment::step)
    .def("get_state", &get_state<RamEnvironment>);

  /* ================ Screen Environment ================ */
  /* we only include this conditionally if OpenGL was found available for linking */

#ifdef INCLUDE_SCREEN_ENV

//  using ScreenEnvironment = agario::env::ScreenEnvironment<true>;
//
//  pybind11::class_<ScreenEnvironment>(module, "ScreenEnvironment")
//    .def(pybind11::init<int, int, bool, int, int, int, screen_len, screen_len>())
//    .def("step", &ScreenEnvironment::step)
//    .def("get_state", [](const ScreenEnvironment &env) {
//
//      auto &observation = env.get_state();
//      auto *data = const_cast<std::uint8_t *>(observation.frame_data());
//      auto shape = observation.shape();
//      auto strides = observation.strides();
//
//      auto format = py::format_descriptor<std::uint8_t>::format();
//      auto buffer = py::buffer_info(data, sizeof(std::uint8_t), format, shape.size(), shape, strides);
//      auto arr = py::array_t<std::uint8_t>(buffer);
//      return arr;
//    })
//    .def("done", &ScreenEnvironment::done)
//    .def("take_action", &ScreenEnvironment::take_action, "x"_a, "y"_a, "act"_a)
//    .def("reset", &ScreenEnvironment::reset)
//    .def("render", &ScreenEnvironment::render);

#endif

}
