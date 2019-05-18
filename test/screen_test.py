"""
File: screen_test
Date: 5/18/19 
Author: Jon Deaton (jdeaton@stanford.edu)
"""

import gym
import gym_agario
import numpy as np

import unittest

env_name = "agario-screen-v0"
env_config = {
    'frames_per_step': 4,
    'arena_size': 1000,
    'num_pellets': 1000,
    'num_viruses': 25,
    'num_bots': 25,
    'pellet_regen': True,
    'screen_len': 1024
}

class ScreenGymTest(unittest.TestCase):

    def test_creation(self):
        env = gym.make(env_name, **env_config)
        assert isinstance(env, gym.Env)

    def test_step(self):
        env = gym.make(env_name, **env_config)
        next_state, reward, done, info = env.step((0.0, 0.0, 0))

        self.assertIsInstance(next_state, np.ndarray)
        self.assertIsInstance(reward, float)
        self.assertIsInstance(done, bool)
        self.assertIsInstance(info, dict)

        expected_shape = (env_config['frames_per_step'], env_config['screen_len'], env_config['screen_len'], 3)
        self.assertEqual(next_state.shape, expected_shape)

        self.assertGreater(np.sum(next_state), 0)
        self.assertFalse(np.all(next_state == 255))

    def test_steps(self):
        env = gym.make(env_name, **env_config)
        for _ in range(10):
            next_state, reward, done, info = env.step((0.0, 0.0, 0))

            self.assertIsInstance(next_state, np.ndarray)
            self.assertIsInstance(reward, float)
            self.assertIsInstance(done, bool)
            self.assertIsInstance(info, dict)

            expected_shape = (env_config['frames_per_step'], env_config['screen_len'], env_config['screen_len'], 3)
            self.assertEqual(next_state.shape, expected_shape)

            self.assertGreater(np.sum(next_state), 0)
            self.assertFalse(np.all(next_state == 255))


if __name__ == "__main__":
    unittest.main()
