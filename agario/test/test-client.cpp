#include <client/client.hpp>
#include <rendering/shader.hpp>
#include <core/Player.hpp>

#include <gtest/gtest.h>

namespace {

  TEST(RenderTest, Once) {
    agario::Client client;

    agario::Player<true> jon(0, "Jon", agario::color::blue);

    agario::distance x = 100;
    agario::distance y = 100;
    agario::mass mass = 25;
    jon.add_cell(x, y, mass);
    jon.add_cell(x + 10, y + 10, mass);
    jon.add_cell(x + 10, y - 10, mass);


    agario::Player<true> austin(0, "Austin", agario::color::yellow);
    austin.add_cell(x, y, mass);
    austin.add_cell(x - 10, y + 10, mass);
    austin.add_cell(x - 10, y - 10, mass);

    client.set_player(jon);
    client.add_player(austin);

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