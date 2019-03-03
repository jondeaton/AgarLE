#ifndef AGARIO_UTILS_HPP
#define AGARIO_UTILS_HPP

#include "types.hpp"

namespace Agario {

  distance radius_conversion(mass mass);
  mass mass_conversion(distance radius);
  template <typename T> T div_round_up(T num, T denom) {
    return static_cast<T>((num + denom - 1) / denom);
  }


  // todo: new type speed?
  float max_speed(Agario::mass mass);

}

#endif //AGARIO_UTILS_HPP
