"""
File: Env
Date: 2019-07-30 
Author: Jon Deaton (jdeaton@stanford.edu)
"""

import gym
from gym import error, spaces, utils
import numpy as np
from collections import namedtuple

import agarle

FullObservation = namedtuple('Observation', ['pellets', 'viruses', 'foods', 'agent', 'others'])


class AgarioEnv(gym.Env):
    metadata = {'render.modes': ['human']}

    def __init__(self, obs_type='screen', **kwargs):
        super(AgarioEnv, self).__init__()

        if obs_type not in ("ram", "screen", "grid", "full"):
            raise ValueError(obs_type)

        self._env, self.observation_space = self._make_environment(obs_type, kwargs)
        self.steps = None
        self.obs_type = obs_type

        target_space = spaces.Box(low=0, high=self.arena_size, shape=(2,))
        self.action_space = spaces.Tuple((target_space, spaces.Discrete(3)))

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
        assert self.steps is not None, "Cannot call env.step() before calling reset()"

        x, y, game_act = action
        self._env.take_action(x, y, game_act)

        reward = self._env.step()
        done = self._env.done()

        observation = None if done else self._make_observation()
        self.steps += 1
        return observation, reward, done, {'steps': self.steps}

    def reset(self):
        """ resets the environment
        :return: the state of the environment at the beginning
        """
        self.steps = 0
        self._env.reset()
        return self._make_observation()

    def render(self, mode='human'):
        self._env.render()

    def __del__(self):
        pass

    def _make_observation(self):
        state = self._env.get_state()

        if self.obs_type == "full":
            # full observation type requires this special wrapper
            observation = FullObservation(pellets=state[0], viruses=state[1],
                                          foods=state[2], agent=state[3], others=state[4:])

        elif self.obs_type in ("grid",):
            # NCHW to NHWC
            observation = np.transpose(state, [1, 2, 0])

        else:
            observation = state

        return observation

    def _make_environment(self, obs_type, kwargs):
        assert obs_type in ("ram", "screen", "grid", "full")

        args = self._get_env_args(kwargs)

        if obs_type == "grid":
            num_frames = kwargs.get("num_frames", 2)
            grid_size = kwargs.get("grid_size", 128)
            observe_cells = kwargs.get("observe_cells", True)
            observe_others = kwargs.get("observe_others", True)
            observe_viruses = kwargs.get("observe_viruses", True)
            observe_pellets = kwargs.get("observe_pellets", True)

            env = agarle.GridEnvironment(*args)
            env.configure_observation({
                "num_frames": num_frames,
                "grid_size": grid_size,
                "observe_cells": observe_cells,
                "observe_others": observe_others,
                "observe_viruses": observe_viruses,
                "observe_pellets": observe_pellets
            })

            channels, width, height = env.observation_shape()
            shape = (width, height, channels)
            observation_space = spaces.Box(-np.inf, np.inf, shape, dtype=np.int32)

        elif obs_type == "ram":
            env = agarle.RamEnvironment(*args)
            shape = env.observation_shape()
            observation_space = spaces.Box(-np.inf, np.inf, shape)

        elif obs_type == "screen":
            # the screen environment requires the additional
            # arguments of screen width and height. We don't use
            # the "configure_observation" design here because it would
            # introduce some ugly work-arounds and layers of indirection
            # in the underlying C++ code
            screen_len = kwargs.get("screen_len", 256)
            args += (screen_len, screen_len)
            try:
                env = agarle.ScreenEnvironment(*args)
            except AttributeError:
                raise error.Error("Screen environment not available")

            # todo: use env.observation_shape() ?
            shape = 4, screen_len, screen_len, 3
            observation_space = spaces.Box(low=0, high=255, shape=shape, dtype=np.uint8)

        elif obs_type == "full":
            env = agarle.FullEnvironment(*args)
            observation_space = spaces.Dict({
                "pellets": spaces.Space(shape=(None, 2)),
                "viruses": spaces.Space(shape=(None, 2)),
                "foods":   spaces.Space(shape=(None, 2)),
                "agent":   spaces.Space(shape=(None, 5)),
                "others":  spaces.Space(shape=(None, None, 5))
            })

        return env, observation_space

    def _get_env_args(self, kwargs):
        """
        creates a set of positional arguments to pass to the learning environment
        which specify how difficult to make the environment
        :param kwargs: arguments from the instantiation of t
        :return: list of arguments to the underlying environment
        """
        difficulty = kwargs.get("difficulty", "normal")
        if difficulty not in ["normal", "empty", "trivial"]:
            raise ValueError(difficulty)

        # default values for the "normal"
        ticks_per_step = 4
        arena_size = 1000
        num_pellets = 1000
        num_viruses = 25
        num_bots = 25
        pellet_regen = True

        if difficulty == "normal":
            pass  # default

        elif difficulty == "empty":
            num_bots = 0

        elif difficulty == "trivial":
            arena_size = 50
            num_pellets = 100
            num_viruses = 0
            num_bots = 0

        # now, override any of the defaults with those from the arguments
        self.ticks_per_step  = kwargs.get("ticks_per_step", ticks_per_step)
        self.arena_size      = kwargs.get("arena_size", arena_size)
        self.num_pellets     = kwargs.get("num_pellets", num_pellets)
        self.num_viruses     = kwargs.get("num_viruses", num_viruses)
        self.num_bots        = kwargs.get("num_bot", num_bots)
        self.pellet_regen    = kwargs.get("pellet_regen", pellet_regen)

        return self.ticks_per_step, self.arena_size, \
               self.pellet_regen, self.num_pellets, \
               self.num_viruses, self.num_bots

