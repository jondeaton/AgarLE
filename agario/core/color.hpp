#pragma once

namespace agario {
  enum color { red, orange, yellow, green, blue, purple, last };

  float red_color[] = {1.0, 0.0, 0.0};
  float blue_color[] = {0.0, 0.0, 1.0};
  float green_color[] = {0.0, 1.0, 0.0};
  float orange_color[] = {1.0, 0.65, 0.0};
  float purple_color[] = {0.6, 0.2, 0.8};
  float yellow_color[] = {1.0, 1.0, 0.0};
  float black_color[] = {0.0, 0.0, 0.0};

  agario::color random_color() {
    return static_cast<enum color>(rand() % agario::color::last);
  }

}