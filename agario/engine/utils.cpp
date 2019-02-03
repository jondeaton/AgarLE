#include "utils.hpp"
#include "types.hpp"
#include <cmath>

#define MASS_AREA_RADIO 1.0

namespace Agario {

  length radius_conversion(mass mass) {
    auto area = mass / MASS_AREA_RADIO;
    return (length) std::sqrt(area / M_PI);
  }

  mass mass_conversion(length radius) {
    return (mass) std::round(MASS_AREA_RADIO * M_PI * std::pow(radius, 2));
  }

  template <typename T>
  T div_round_up(T num, T denom) {
    return (num + denom - 1) / denom;
  }

}
