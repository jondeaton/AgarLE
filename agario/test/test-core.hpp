#pragma once

#include <gtest/gtest.h>
#include <engine/Engine.hpp>
#include <test/renderable.hpp>

namespace {

  /* =========== Coordinate =========== */

  TEST(Location, Construct) {
    agario::distance x = 3.4;
    agario::distance y = 5.6;
    agario::Coordinate<float> s(x, y);

    EXPECT_FLOAT_EQ(s.x, x);
    EXPECT_FLOAT_EQ(s.y, y);
  }

  TEST(Location, Construct2) {
    agario::distance x = 3.4;
    agario::distance y = 5.6;
    agario::Coordinate<agario::distance> s = { x, y };

    EXPECT_FLOAT_EQ(s.x, x);
    EXPECT_FLOAT_EQ(s.y, y);
  }

  TEST(Location, Construct3) {
    agario::Coordinate<agario::distance> s = { 1.2, 3.4 };

    EXPECT_FLOAT_EQ(s.x, 1.2);
    EXPECT_FLOAT_EQ(s.y, 3.4);
  }

  TEST(Location, Construct4) {
    agario::Coordinate<agario::distance> s;

    EXPECT_FLOAT_EQ(s.x, 0) << "Coordinate not initilaized x = 0";
    EXPECT_FLOAT_EQ(s.y, 0) << "Coordinate not initialized y = 0";

    agario::distance x = 3.4;
    agario::distance y = 5.6;

    s = { x, y };

    EXPECT_FLOAT_EQ(s.x, x) << "Coordinate x did not update";
    EXPECT_FLOAT_EQ(s.y, y) << "Coordinate y did not update";
  }

  TEST(Location, Increment) {
    agario::distance x = 3.4;
    agario::distance y = 5.6;
    agario::Coordinate<agario::distance> s;

    for (int i = -5; i < 5; i++) {
      s = { x, y };

      agario::distance dx = i / 3.0;
      agario::distance dy = (2 - i) / 3.0;
      agario::Coordinate<agario::distance> ds = { dx, dy };

      s += ds;

      ASSERT_FLOAT_EQ(s.x, x + dx) << "Coordinate x bad increment";
      ASSERT_FLOAT_EQ(s.y, y + dy )<< "Coordinate y bad increment";;
    }
  }

  TEST(Location, Decrement) {
    agario::distance x = 3.4;
    agario::distance y = 5.6;
    agario::Coordinate<agario::distance> s;

    for (int i = -5; i < 5; i++) {
      s = { x, y };

      agario::distance dx = i / 3.0;
      agario::distance dy = (2 - i) / 3.0;
      agario::Coordinate<agario::distance> ds = { dx, dy };

      s -= ds;

      ASSERT_FLOAT_EQ(s.x, x - dx) << "Coordinate x bad decrement";
      ASSERT_FLOAT_EQ(s.y, y - dy )<< "Coordinate y bad decrement";
    }
  }
  
  /* =========== Velocity =========== */

}
