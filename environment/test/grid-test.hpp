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

using namespace agario::env;

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
    EnvTest() : env(4, 4, 1000,
      true, 1000,
      25, 25) { }
    void SetUp() override {
      env.configure_observation(2, 128,
        true, true,
        true, true);
    }
    void TearDown() override { }
    GridEnvironment env;
  };

  TEST_F(EnvTest, create_env) {
    EXPECT_NO_THROW(SetUp());
  }

  /* make sure that you can take actions */
  TEST_F(EnvTest, take_actions) {
    SetUp();
    std::vector<Action> actions;
    actions.reserve(env.num_agents());

    for (int i = 0; i < env.num_agents(); i++)
      actions.emplace_back(0.0, 0.0, agario::action::none);
    EXPECT_NO_THROW(    env.take_actions(actions));
  }

  /* make sure that it throws when number of actions != number of agents */
  TEST_F(EnvTest, take_action_wrong_size) {
    SetUp();
    std::vector<Action> actions;
    actions.reserve(env.num_agents());

    EXPECT_THROW(env.take_actions(actions), EnvironmentException);

    for (int a = 1; a <= 1 + env.num_agents(); a++) {
      actions.emplace_back(0.0, 0.0, agario::action::none);
      if (a != env.num_agents())
        EXPECT_THROW(env.take_actions(actions), EnvironmentException);
      else
        EXPECT_NO_THROW(env.take_actions(actions));
    }
  }

  TEST_F(EnvTest, reset) {
    SetUp();
    EXPECT_NO_THROW(env.reset());
  }

  TEST_F(EnvTest, step) {
    SetUp();
    std::vector<Action> actions;
    actions.reserve(env.num_agents());
    for (int i = 0; i < env.num_agents(); i++)
      actions.emplace_back(0.0, 0.0, agario::action::none);

    for (int i = 0; i < 100; i++) {
      ASSERT_NO_THROW(env.take_actions(actions));
      auto rewards = env.step();
      ASSERT_EQ(rewards.size(), env.num_agents());
    }
  }

  TEST_F(EnvTest, get_state) {
    SetUp();
    std::vector<Action> actions;
    actions.reserve(env.num_agents());
    for (int i = 0; i < env.num_agents(); i++)
      actions.emplace_back(0.0, 0.0, agario::action::none);

    for (int i = 0; i < 100; i++) {
      ASSERT_NO_THROW(env.take_actions(actions));
      auto _ = env.step();

      auto &observations = env.get_observations();
      ASSERT_EQ(observations.size(), env.num_agents());
      for (auto &o : observations)
        ASSERT_TRUE(has_non_zero(o));
    }
  }

}
