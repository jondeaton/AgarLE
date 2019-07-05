#pragma once

#include <gtest/gtest.h>
#include <engine/Engine.hpp>

namespace {

  TEST(EntityTest, MakeCell) {
    agario::distance x = 100;
    agario::distance y = 125;
    agario::mass mass = 25;

    agario::Cell<false> cell(x, y, mass);
    EXPECT_EQ(cell.x, x);
    EXPECT_EQ(cell.y, y);
    EXPECT_EQ(cell.mass(), mass);
  }

  TEST(EntityTest, CustomCell) {
    agario::distance x = 100;
    agario::distance y = 125;
    agario::mass mass = 25;

    agario::Cell<false> cell(x, y, mass);
    EXPECT_EQ(cell.x, x);
    EXPECT_EQ(cell.y, y);
    EXPECT_EQ(cell.mass(), mass);
  }

  TEST(PlayerTest, StartsWithCell) {
    agario::Player<false> player(0, "TestPlayer");
    EXPECT_EQ(player.cells.size(), 1ul);
  }

  TEST(PlayerTest, NotDead) {
    agario::Player<false> player(0, "TestPlayer");
    EXPECT_FALSE(player.dead());
  }

  TEST(PlayerTest, Kill) {
    agario::Player<false> player(0, "TestPlayer");
    EXPECT_FALSE(player.dead());
    player.kill();
    EXPECT_TRUE(player.dead());
  }

  TEST(PlayerTest, AddCell) {
    agario::Player<false> player(0, "TestPlayer");
    player.kill();

    agario::distance x = 100;
    agario::distance y = 125;
    agario::mass mass = 25;

    player.add_cell(x, y, mass);
    ASSERT_EQ(player.cells.size(), 2ul);

    EXPECT_EQ(player.cells.front().mass(), mass);
    EXPECT_EQ(player.cells.front().x, x);
    EXPECT_EQ(player.cells.front().y, y);
  }

  TEST(PlayerTest, SimpleLocation) {
    agario::Player<false> player(0, "TestPlayer");
    player.kill(); // reset cells

    EXPECT_EQ(player.cells.size(), 0ul);

    agario::distance x = 100;
    agario::distance y = 100;
    agario::mass mass = 25;

    player.add_cell(x, y, mass);
    EXPECT_EQ(player.x(), x);
    EXPECT_EQ(player.y(), y);
  }

  TEST(PlayerTest, Location) {
    agario::Player<false> player(0, "TestPlayer");

    EXPECT_EQ(player.cells.size(), 0ul);

    agario::distance x = 100;
    agario::distance y = 100;
    agario::mass mass = 25;

    player.add_cell(x, y, mass);
    player.add_cell(x + 2, y + 2, mass);

    EXPECT_EQ(player.x(), x + 1);
    EXPECT_EQ(player.y(), y + 1);
  }

}
