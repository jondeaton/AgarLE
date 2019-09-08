"""
File: ram_env_test
Date: 9/7/19 
Author: Jon Deaton (jonpauldeaton@gmail.com)
"""


import gym, gym_agario

import numpy as np
import unittest

env_name = "agario-full-v0"


class RamGymTest(unittest.TestCase):

    def test_creation(self):
        env = self._make_env()
        self.assertIsInstance(env, gym.Env)

    def test_reset(self):
        env = self._make_env()
        state = env.reset()
        self._assertValidState(env, state)

    def test_shape(self):
        env = self._make_env()
        state, reward, done, info = env.step((0.0, 0.0, 0))
        self._assertValidState(env, state)

    def test_steps(self):
        env = self._make_env()

        for _ in range(10):
            state, reward, done, info = env.step((0.0, 0.0, 0))
            self._assertValidState(env, state)

    def _assertValidState(self, env, state):
        # makes sure that the state is correctly formed
        self.assertIsInstance(state, np.ndarray)
        self.assertEqual(state.dtype, np.int32, "data type: %s" % state.dtype)
        self.assertEqual(state.shape, env.observation_space.shape)

        # this one is really important
        self.assertLess(state.min(), state.max())  # not all just one value

    def _make_env(self):
        env_config = {
            'ticks_per_step': 4,
            'arena_size': 1000,
            'num_pellets': 1000,
            'num_viruses': 25,
            'num_bots': 25,
            'pellet_regen': True,
            'grid_size': 128,
            'observe_cells': True,
            'observe_others': True,
            'observe_viruses': True,
            'observe_pellets': True
        }
        return gym.make("agario-ram-v0", **env_config)



if __name__ == "__main__":
    unittest.main()
