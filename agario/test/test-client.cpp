#include <client/client.hpp>
#include <client/shader.hpp>
#include <core/Player.hpp>

#include <gtest/gtest.h>

namespace {

  TEST(RenderTest, Once) {
   Agario::Client client;

   Agario::Player<true> player(0, "Jon");

   Agario::distance x = 100;
   Agario::distance y = 100;
   Agario::mass mass = 25;
   player.add_cell(x, y, mass);

   client.set_player(player);
   client.game_loop();
  }

//  TEST(RenderTest, Simple) {
//   AgarioClient client;
//   client.game_loop(10);
//  }

}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}