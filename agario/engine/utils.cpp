
#include "utils.hpp"
#include "types.hpp"
#include "settings.hpp"

#include <cmath>

namespace Agario {

  length radius_conversion(mass mass) {
    auto area = mass / MASS_AREA_RADIO;
    return (length) std::sqrt(area / M_PI);
  }

  Agario::mass mass_conversion(length radius) {
    return (mass) std::round(MASS_AREA_RADIO * M_PI * std::pow(radius, 2));
  }

}
