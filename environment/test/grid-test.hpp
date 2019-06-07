#pragma once

#include <gtest/gtest.h>
#include "envs/GridEnvironment.hpp"

static constexpr bool renderable =
#ifdef RENDERABLE
  true
#else
  false
#endif
  ;

namespace {

  typedef agario::env::GridEnvironment<int, renderable> GridEnvironment;
  typedef GridEnvironment::dtype dtype;

  class EnvTest : public testing::Test {
  protected:
    EnvTest() : env(4, 1000, true, 1000, 25, 25) { }
    void SetUp() override { }
    void TestDown() { }

    GridEnvironment env;
  };

  TEST_F(EnvTest, create_engine) {
    SetUp();
    EXPECT_TRUE(true);
  }

}
