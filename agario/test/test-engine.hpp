#ifndef AGARIO_ENGINE_TEST_HPP
#define AGARIO_ENGINE_TEST_HPP

#include <gtest/gtest.h>
#include <engine/Engine.hpp>

namespace {

  TEST(EntityTest, MakeMovingBall) {
//    Agario::MovingBall mb(Agario::Location(4, 4), Agario::Velocity(2,2 ));
  }

  TEST(EntityTest, MakeCell) {
    Agario::distance x = 100;
    Agario::distance y = 125;
    Agario::mass mass = 25;

    Agario::Cell<false> cell(x, y, mass);
    EXPECT_EQ(cell.x, x);
    EXPECT_EQ(cell.y, y);
    EXPECT_EQ(cell.mass(), mass);
  }

  TEST(PlayerTest, StartsEmpty) {
    Agario::Player<false> player(0, "TestPlayer");
    EXPECT_EQ(player.cells.size(), 0ul);
  }

  TEST(PlayerTest, AddCell) {
    Agario::Player<false> player(0, "TestPlayer");

    Agario::distance x = 100;
    Agario::distance y = 125;
    Agario::mass mass = 25;

    player.add_cell(x, y, mass);

    ASSERT_EQ(player.cells.size(), 1ul);

    EXPECT_EQ(player.cells[0].mass(), mass);
    EXPECT_EQ(player.cells[0].x, x);
    EXPECT_EQ(player.cells[0].y, y);
  }

  TEST(PlayerTest, Location) {
    Agario::Player<false> player(0, "TestPlayer");

    EXPECT_EQ(player.cells.size(), 1ul);

    Agario::distance x = 100;
    Agario::distance y = 100;
    Agario::mass mass = 25;

    player.add_cell(x, y, mass);
    EXPECT_EQ(player.x(), x);
    EXPECT_EQ(player.y(), y);
  }


  class EngineTest : public testing::Test {
  protected:
    EngineTest() : engine() { }
    void SetUp() { }
    void TestDown() { }

    Agario::Engine engine;
  };

  TEST_F(EngineTest, create_engine) {
    SetUp();
    EXPECT_TRUE(true);
  }

}

#endif //AGARIO_ENGINE_TEST_HPP
