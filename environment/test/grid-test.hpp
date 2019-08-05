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

  using GridEnvironment = agario::env::GridEnvironment<int, renderable>;
  using dtype = GridEnvironment::dtype;
  using Observation = GridEnvironment::Observation;

  /* checks to make sure that the observation is not zero-filled */
  bool has_non_zero(const Observation &obs) {
    bool found = false;
    const dtype *data = obs.data();
    for (int i = 0; i < obs.length(); i++) {
      if (data[i] != 0)
        found = true;
    }
    return found;
  }

  /* ===================== Tests ===================== */
  class EnvTest : public testing::Test {
  protected:
    EnvTest() : env(4, 1000, true, 1000, 25, 25) { }
    void SetUp() override {
      env.configure_observation(2, 128, true, true, true, true);
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
    EXPECT_TRUE(has_non_zero(state));
  }

  TEST_F(EnvTest, multi_get_state) {
    SetUp();
    for (int i = 0; i < 32; i++) {
      env.take_action(0, 0, 0);
      auto r = env.step();
      auto &state = env.get_state();
      ASSERT_TRUE(has_non_zero(state));
    }
  }

}
