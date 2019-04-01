
#include "utils.hpp"
#include "core/types.hpp"
#include "settings.hpp"

#include <cmath>

namespace Agario {

  distance radius_conversion(mass mass) {
    auto area = mass / MASS_AREA_RADIO;
    return (distance) std::sqrt(area / M_PI);
  }

  Agario::mass mass_conversion(distance radius) {
    return (mass) std::round(MASS_AREA_RADIO * M_PI * std::pow(radius, 2));
  }

}