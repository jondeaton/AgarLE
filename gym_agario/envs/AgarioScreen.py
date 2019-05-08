"""
File: ScreenEnvironment
Date: 5/5/19 
Author: Jon Deaton (jdeaton@stanford.edu)
"""

import gym
from gym import error, spaces, utils
from gym.utils import seeding


import gym
from gym import error, spaces, utils

from collections import namedtuple

import agario_screen_env

class AgarioScreen(gym.Env):
    metadata = {'render.modes': ['human']}

    def __init__(self):
        self.viewer = None
        self.server_process = None
        self.server_port = None

        self.action_space = spaces.Tuple((
            spaces.Box(low=0, high=1, shape=2), # x, y target
            spaces.MultiBinary(1), # split?
            spaces.MultiBinary(1))) # feed?

        self._env = agario_screen_env.Environment(4)

        self.prev_status = 0
        self.status = 0

    def __del__(self):
        pass

    def step(self, action):
        """

        Parameters
        ----------
        action : Action, including

        Returns
        -------
        observation, reward, episode_over, info : tuple
            observation (object) :
                an environment-specific object representing your observation of
                the environment.
            reward (float) :
                amount of reward achieved by the previous action. The scale
                varies between environments, but the goal is always to increase
                your total reward.
            episode_over (bool) :
                whether it's time to reset the environment again. Most (but not
                all) tasks are divided up into well-defined episodes, and done
                being True indicates the episode has terminated. (For example,
                perhaps the pole tipped too far, or you lost your last life.)
            info (dict) :
                 diagnostic information useful for debugging. It can sometimes
                 be useful for learning (for example, it might contain the raw
                 probabilities behind the environment's last state change).
                 However, official evaluations of your agent are not allowed to
                 use this for learning.
        """

        self._env.take_action(action)

        reward = self._env.step()
        observation = self._env.get_state()
        episode_over = self._env.done()

        return observation, reward, episode_over, {}

    def reset(self):
        self._env.reset()

    def render(self, mode='human'):
        self._env.render()
