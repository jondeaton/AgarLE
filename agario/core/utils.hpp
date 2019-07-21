#pragma once

#include "agario/core/types.hpp"
#include "agario/core/settings.hpp"

namespace agario {

  agario::distance radius_conversion(mass mass) {
    auto area = mass / MASS_AREA_RADIO;
    return (distance) std::sqrt(area / M_PI);
  }

  agario::mass mass_conversion(distance radius) {
    auto area = M_PI * std::pow(radius, 2);
    return static_cast<agario::mass>(std::round(MASS_AREA_RADIO * area));
  }

  template <typename T>
  T clamp(T x, T low, T high) {
    return std::max<T>(std::min<T>(x, high), low);
  }

  template <typename T> T div_round_up(T num, T denom) {
    return static_cast<T>((num + denom - 1) / denom);
  }

}
