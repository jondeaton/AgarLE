#include <client/client.hpp>
#include <rendering/shader.hpp>
#include <core/Player.hpp>

#include <gtest/gtest.h>

namespace {

  TEST(RenderTest, Once) {
    agario::Client client;

    typedef agario::Player<true> Player;

    agario::distance x = 100;
    agario::distance y = 100;
    agario::mass mass = 25;


    Player jon(0, "Jon", agario::color::blue);
    jon.add_cell(x, y, mass);
    jon.add_cell(x + 10, y + 10, mass);
    jon.add_cell(x + 10, y - 10, mass);


    Player austin(1, "Austin", agario::color::purple);
    austin.add_cell(x, y, mass);
    austin.add_cell(x - 10, y + 10, mass);
    austin.add_cell(x - 10, y - 10, mass);

    client.set_player(jon);
    client.add_player(austin);

    client.game_loop();
  }

}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}