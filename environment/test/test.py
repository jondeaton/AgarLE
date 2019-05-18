#!/usr/bin/env python

import unittest
import numpy as np

import agario_full_env

def get_env():
    return agario_full_env.Environment(4, 0, True, 0, 0, 0)

class FullEnvTest(unittest.TestCase):

    def test_creation(self):
        env = get_env()
        self.assertIsInstance(env, agario_full_env.Environment)

    def test_step(self):
        env = get_env()
        r = env.step()
        self.assertIsInstance(r, float)

    def test_steps(self):
        env = get_env()
        for _ in range(10):
            r = env.step()
            self.assertIsInstance(r, float)

    def test_state(self):
        env = get_env()
        state = env.get_state()

        # state is [viruses, pellets, food, me, others]
        self.assertIsInstance(state, list)
        self.assertGreaterEqual(len(state), 4)
        for el in state:
            self.assertIsInstance(el, np.ndarray)

        for _ in range(10):
            r = env.step()
            state = env.get_state()

            self.assertIsInstance(state, list)
            self.assertGreaterEqual(len(state), 4)
            for el in state:
                self.assertIsInstance(el, np.ndarray)

    def test_changing(self):
        env = get_env()
        state = env.get_state()

        me = state[4]



if __name__ == "__main__":
    unittest.main()