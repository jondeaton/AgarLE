#ifndef AGARIO_ENGINE_TEST_HPP
#define AGARIO_ENGINE_TEST_HPP

#include <gtest/gtest.h>
#include <engine/Engine.hpp>

namespace {

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
