#ifndef AGARIO_UTILS_HPP
#define AGARIO_UTILS_HPP

#include "types.hpp"

namespace Agario {
  length radius_conversion(mass mass);
  mass mass_conversion(length radius);
  template <typename T> T div_round_up(T num, T denom) {
    return static_cast<T>((num + denom - 1) / denom);
  }
}

#endif //AGARIO_UTILS_HPP
