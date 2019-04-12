#pragma once

#include <core/types.hpp>
#include <engine/settings.hpp>

namespace agario {

  float max_speed(agario::mass mass) {
    return CELL_MAX_SPEED * CELL_MIN_SIZE / (float) mass;
  }

}