#pragma once

#include <gtest/gtest.h>
#include <engine/Engine.hpp>

#include <test/renderable.hpp>

namespace {

  class EngineTest : public testing::Test {
  protected:
    EngineTest() : engine() {}
    void SetUp() {}
    void TearDown() {}

    agario::Engine<renderable> engine;
  };

  TEST_F(EngineTest, Construct) {
    SetUp();
    EXPECT_TRUE(true);
  }

}
