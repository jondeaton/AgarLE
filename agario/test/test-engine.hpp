#pragma once

#include <gtest/gtest.h>
#include <engine/Engine.hpp>

namespace {

  class EngineTest : public testing::Test {
  protected:
    EngineTest() : engine() {}
    void SetUp() {}
    void TearDown() {}

    agario::Engine<false> engine;
  };

  TEST_F(EngineTest, Construct) {
    SetUp();
    EXPECT_TRUE(true);
  }

}
