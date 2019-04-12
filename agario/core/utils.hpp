#pragma once

#include "core/types.hpp"

namespace agario {

  distance radius_conversion(mass mass);
  mass mass_conversion(distance radius);

  template <typename T> T div_round_up(T num, T denom) {
    return static_cast<T>((num + denom - 1) / denom);
  }

  // todo: new type speed?
  float max_speed(agario::mass mass);
}
