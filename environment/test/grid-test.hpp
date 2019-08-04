#pragma once

#include <gtest/gtest.h>

#include "environment/envs/GridEnvironment.hpp"

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
    void SetUp() override {
      env.configure_observation(128, true, true, true, true);
    }
    void TearDown() override { }

    GridEnvironment env;
  };

  TEST_F(EnvTest, create_env) {
    SetUp();
  }

  TEST_F(EnvTest, step) {
    SetUp();
    env.take_action(0, 0, 0);
    auto r = env.step();
  }

  TEST_F(EnvTest, get_state) {
    SetUp();
    env.take_action(0, 0, 0);
    auto r = env.step();
    auto &state = env.get_state();
  }

  TEST_F(EnvTest, multi_get_state) {
    SetUp();
    for (int i = 0; i < 32; i++) {
      env.take_action(0, 0, 0);
      auto r = env.step();
      auto &state = env.get_state();
    }
  }

}
