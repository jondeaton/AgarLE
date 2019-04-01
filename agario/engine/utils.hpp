#pragma once

#include <core/types.hpp>
#include <engine/settings.hpp>

namespace Agario {

  float max_speed(Agario::mass mass) {
    return CELL_MAX_SPEED * CELL_MIN_SIZE / (float) mass;
  }

}