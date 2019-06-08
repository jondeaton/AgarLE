"""
File: grid_test
Date: 6/7/19 
Author: Jon Deaton (jdeaton@stanford.edu)
"""

import gym
import gym_agario
import numpy as np

import unittest

env_config = {
    'frames_per_step': 4,
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

class GridGymTest(unittest.TestCase):

    def test_creation(self):
        env = gym.make("agario-grid-v0", **env_config)
        assert isinstance(env, gym.Env)

    def test_shape(self):
        env = gym.make("agario-grid-v0", **env_config)
        state, reward, done, info = env.step((0.0, 0.0, 0))

        self.assertValidState(state)

    def test_steps(self):
        env = gym.make("agario-grid-v0", **env_config)
        for _ in range(10):
            state, reward, done, info = env.step((0.0, 0.0, 0))

            self.assertValidState(state)

    def assertValidState(self, state):
        self.assertIsInstance(state, np.ndarray)
        self.assertEqual(state.dtype, np.int32, "data type: %s" % state.dtype)
        self.assertEqual(state.shape, (4, 128, 128))

        self.assertGreaterEqual(state.min(), -1)
        self.assertLess(state.max(), 1000)

        self.assertLess(state.min(), state.max()) # not all just one value

if __name__ == "__main__":
    unittest.main()
