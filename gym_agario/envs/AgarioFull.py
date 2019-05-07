#!/usr/bin/env python
"""
File: AgarEnv
Date: 1/27/19 
Author: Jon Deaton (jdeaton@stanford.edu)
"""

import gym
from gym import error, spaces, utils
import numpy as np

from collections import namedtuple

import agario_full_env

Observation = namedtuple('Observation', ['viruses', 'foods', 'pellets', 'agent', 'others'])

class AgarioFull(gym.Env):
    metadata = {'render.modes': ['human']}

    def __init__(self):
        self.viewer = None
        self.server_process = None
        self.server_port = None

        self.action_space = spaces.Tuple((spaces.Box(low=0, high=1, shape=(2,)),
                                          spaces.MultiBinary(1),
                                          spaces.MultiBinary(1)))

        self.observation_space = spaces.Dict({
            "viruses": spaces.Space(shape=(None, 2)),
            "foods": spaces.Space(shape=(None, 2)),
            "pellets": spaces.Space(shape=(None, 2)),
            "agent": spaces.Space(shape=(None, 5)),
            "others": spaces.Space(shape=(None, None, 5))
        })

        self._env = agario_full_env.Environment(4)

        self.prev_status = 0
        self.status = 0

    def __del__(self):
        pass

    def step(self, action):
        """ take an action in the environment, advancing the environment
        along until the next time step
        :param action: (x, y, a) where `x`, `y` are in [0, 1] and `a` is
        in {0, 1, 2} corresponding to nothing, split, feed, respectively.
        :return: tuple of - observation, reward, episode_over
            observation (object) : the next state of the world.
            reward (float) : reward gained during the time step
            episode_over (bool) : whether the game is over or not
            info (dict) : diagnostic information (currently empty)
        """
        x, y, game_act = action
        self._env.take_action(x, y, game_act)

        reward = self._env.step()
        state = self._env.get_state()

        observation = Observation(viruses=state[0], pellets=state[1],
                                       foods=state[2], agent=state[3], others=state[4:])

        episode_over = self._env.done()

        return observation, reward, episode_over, {}

    def reset(self):
        """
        resets the environment
        :return: None
        """
        self._env.reset()

    def render(self, mode='human'):
        self._env.render()
