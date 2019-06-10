"""
File: AgarioGrid
Date: 6/7/19 
Author: Jon Deaton (jdeaton@stanford.edu)
"""


import gym
from gym import error, spaces, utils
import numpy as np

import agario_grid_env

class GridEnv(gym.Env):
    metadata = {'render.modes': ['human']}

    def __init__(self, frames_per_step=4, arena_size=1000, num_pellets=1000,
                 num_viruses=25, num_bots=25, pellet_regen=True, grid_size=128,
                 observe_cells=True, observe_others=True, observe_viruses=True,
                 observe_pellets=True):
        super(GridEnv, self).__init__()
        self.action_space = spaces.Tuple((spaces.Box(low=-1, high=1, shape=(2,)),
                                          spaces.MultiBinary(1),
                                          spaces.MultiBinary(1)))

        num_channels = int(observe_cells + observe_others + observe_viruses + observe_pellets)
        self.observation_space = spaces.Box(-np.inf, np.inf, (num_channels, grid_size, grid_size), dtype=np.int32)

        # create and configure environment
        args = (frames_per_step, arena_size, pellet_regen, num_pellets, num_viruses, num_bots)
        self._env = agario_grid_env.GridEnvironment(*args)
        self._env.configure_observation({
            "grid_size": grid_size,
            "observe_cells": observe_cells,
            "observe_others": observe_others,
            "observe_viruses": observe_viruses,
            "observe_pellets": observe_pellets
        })

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
        episode_over = self._env.done()

        if episode_over:
            state = None
        else:
            state = self._env.get_state()

        return state, reward, episode_over, {}

    def reset(self):
        """
        resets the environment
        :return: None
        """
        self._env.reset()

    def render(self, mode='human'):
        self._env.render()

    def __del__(self):
        pass
