#pragma once

#include <gtest/gtest.h>
#include <engine/Engine.hpp>
#include <test/renderable.hpp>
#include <random>

namespace {

  /* =========== Coordinate =========== */

  TEST(Coordinate, ConstructNone) {
    agario::Coordinate<agario::distance> s;

    EXPECT_FLOAT_EQ(s.x, 0) << "Coordinate x not zero initialized";
    EXPECT_FLOAT_EQ(s.y, 0) << "Coordinate y not zero initialized";
  }

  TEST(Coordinate, Construct) {
    agario::distance x = 3.4;
    agario::distance y = 5.6;
    agario::Coordinate<agario::distance> s(x, y);

    EXPECT_FLOAT_EQ(s.x, x) << "Coordinate x not stored";
    EXPECT_FLOAT_EQ(s.y, y) << "Coordinate y not stored";
  }

  TEST(Coordinate, Construct2) {
    agario::distance x = 3.4;
    agario::distance y = 5.6;
    agario::Coordinate<agario::distance> s = { x, y };

    EXPECT_FLOAT_EQ(s.x, x);
    EXPECT_FLOAT_EQ(s.y, y);
  }

  TEST(Coordinate, Construct3) {
    agario::Coordinate<agario::distance> s = { 1.2, 3.4 };

    EXPECT_FLOAT_EQ(s.x, 1.2);
    EXPECT_FLOAT_EQ(s.y, 3.4);
  }

  TEST(Coordinate, ConstructNegative) {
    agario::distance x = -3.4;
    agario::distance y = -5.6;
    agario::Coordinate<agario::distance> s = { x, y };

    EXPECT_FLOAT_EQ(s.x, x);
    EXPECT_FLOAT_EQ(s.y, y);
  }

  TEST(Coordinate, Construct4) {
    agario::Coordinate<agario::distance> s;

    agario::distance x = 3.4;
    agario::distance y = 5.6;

    s = { x, y };

    EXPECT_FLOAT_EQ(s.x, x) << "Coordinate x did not update";
    EXPECT_FLOAT_EQ(s.y, y) << "Coordinate y did not update";
  }

  TEST(Coordinate, Increment) {
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

  TEST(Coordinate, Decrement) {
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

  TEST(Coordinate, Norm) {
    for (int i = 0; i < 10; i ++) {
      agario::distance x = i - 5;
      agario::distance y = i;
      agario::Coordinate<agario::distance> s(x, y);

      ASSERT_FLOAT_EQ(s.norm(), std::sqrt(x * x + y * y)) << "Incorrect norm";
      ASSERT_FLOAT_EQ(s.norm_sqr(), s.norm() * s.norm())  << "Incorrect norm squared";

      auto z = s;
      z.normalize();

      auto normed = s.normed();

      ASSERT_FLOAT_EQ(normed.x, z.x) << "Bad normalized x";
      ASSERT_FLOAT_EQ(normed.y, z.y) << "Bad normalized y";
    }
  }

  TEST(Coordinate, Distance) {
    for (int i = 0; i < 10; i ++) {
      agario::distance x1 = i - 5;
      agario::distance y1 = i;
      agario::Coordinate<agario::distance> s = { x1, y1 };

      agario::distance x2 = 1.7 * y1 + 7;
      agario::distance y2 = -3.14 * x1 - 10.3;
      agario::Coordinate<agario::distance> z = { x2, y2 };

      ASSERT_GT(s.distance_to(z), 0) << "Norm is negative";
      ASSERT_FLOAT_EQ(s.distance_to(z), z.distance_to(s)) << "argument reversal not equivalent";

      auto dx = x1 - x2;
      auto dy = y1 - y2;
      auto ds = std::sqrt(dx * dx + dy * dy);
      ASSERT_FLOAT_EQ(ds, s.distance_to(z)) << "Incorrect distance";
    }
  }

  TEST(Coordinate, StreamPrint) {
    for (int i = 0; i < 10; i ++) {
      agario::distance x = i - 5;
      agario::distance y = i;
      agario::Coordinate<agario::distance> s = { x, y };

      std::stringstream ss;
      ss << s;

      std::stringstream rss;
      rss << "(" << x << ", " << y << ")";
      ASSERT_EQ(ss.str(), rss.str()) << "Bad string representation";
    }
  }

  TEST(Coordinate, Equality) {
    for (int i = -25; i < 25; i ++) {
      agario::distance x1 = 1 + i % 2;
      agario::distance y1 = i % 2;
      agario::Coordinate<agario::distance> s = { x1, y1 };

      agario::distance x2 = 1 + i % 3;
      agario::distance y2 = i % 3;
      agario::Coordinate<agario::distance> z = { x2, y2 };

      if (z.x == s.x && z.y == s.y)
        EXPECT_EQ(z, s);
      else
        EXPECT_NE(z, s);
    }
  }

  TEST(Coordinate, Math) {
    for (int i = 0; i < 10; i ++) {
      agario::distance x1 = i - 5;
      agario::distance y1 = i;
      agario::Coordinate<agario::distance> s = { x1, y1 };

      agario::distance x2 = 1.7 * y1 + 7;
      agario::distance y2 = -3.14 * x1 - 10.3;
      agario::Coordinate<agario::distance> z = { x2, y2 };

      auto add = s + z;
      ASSERT_EQ(s + z, z + s) << "Addition not commutative";
      ASSERT_FLOAT_EQ(add.x, x1 + x2) << "Addition x incorrect";
      ASSERT_FLOAT_EQ(add.y, y1 + y2) << "Addition y incorrect";

      auto sub1 = s - z;
      auto sub2 = z - s;
      ASSERT_EQ(sub1 * -1, sub2) << "Subtraction inversion incorrect";
      ASSERT_EQ(sub1.x, x1 - x2) << "Subtraction x incorrect";
      ASSERT_EQ(sub1.y, y1 - y2) << "Subtraction y incorrect";

      auto mul_s = s * i;
      ASSERT_EQ(s * i, i * s) << "Multiplication not commutative";
      ASSERT_FLOAT_EQ(mul_s.x, s.x * i);
      ASSERT_FLOAT_EQ(mul_s.y, s.y * i);

      auto div_s = s / (i + 0.1);
      ASSERT_FLOAT_EQ(div_s.x, s.x / (i + 0.1));
      ASSERT_FLOAT_EQ(div_s.y, s.y / (i + 0.1));
    }
  }

  /* =========== Velocity =========== */

  // todo: similarly mind-numbing tests for Velocity class

}
