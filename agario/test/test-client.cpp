#include <client/client.hpp>
#include <rendering/shader.hpp>
#include <core/Player.hpp>

#include <gtest/gtest.h>

namespace {

  TEST(RenderTest, Once) {
    agario::Client client;

    agario::Player<true> player(0, "Jon");

    agario::distance x = 100;
    agario::distance y = 100;
    agario::mass mass = 25;
    player.add_cell(x, y, mass);
    player.add_cell(x + 10, y + 10, mass);
    player.add_cell(x + 10, y - 10, mass);

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