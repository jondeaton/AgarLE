#include <client/client.hpp>
#include <client/shader.hpp>

#include <gtest/gtest.h>

namespace {

  TEST(RenderTest, Once) {
   Agario::Client client;
   
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