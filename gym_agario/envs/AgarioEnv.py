"""
File: AgarioEnv
Date: 5/28/19 
Author: Jon Deaton (jdeaton@stanford.edu)
"""

import gym
from gym import error, spaces, utils
import numpy as np

from collections import namedtuple


FullObservation = namedtuple('FullObservation', ['pellets', 'viruses', 'foods', 'agent', 'others'])

class AgarioFull(gym.Env):
    metadata = {'render.modes': ['human']}

    def __init__(self, observation_type="full", frames_per_step=4, arena_size=1000,
                 num_pellets=1000, num_viruses=25, num_bots=25, pellet_regen=True,
                 screen_len=256):

        self.viewer = None
        self.server_process = None
        self.server_port = None

        self.observation_type = observation_type

        if observation_type == "full":
            import agario_full_env as env_module
            self.action_space = spaces.Tuple((spaces.Box(low=0, high=1, shape=(2,)),
                                              spaces.MultiBinary(1),
                                              spaces.MultiBinary(1)))
            self.observation_space = spaces.Dict({
                "pellets": spaces.Space(shape=(None, 2)),
                "viruses": spaces.Space(shape=(None, 2)),
                "foods":   spaces.Space(shape=(None, 2)),
                "agent":   spaces.Space(shape=(None, 5)),
                "others":  spaces.Space(shape=(None, None, 5))
            })

            args = (frames_per_step, arena_size, pellet_regen, num_pellets, num_viruses, num_bots)
            self._env = env_module.FullEnvironment(*args)

        elif observation_type == "screen":
            import agario_screen_env as env_module

            self.action_space = spaces.Tuple((spaces.Box(low=0, high=1, shape=(2,)),
                                              spaces.MultiBinary(1),
                                              spaces.MultiBinary(1)))

            # todo
            # self.observation_space = None

            args = (frames_per_step, arena_size, pellet_regen,num_pellets,
                    num_viruses, num_bots, screen_len, screen_len)
            self._env = env_module.ScreenEnvironment(*args)

        elif observation_type == "grid":
            raise NotImplementedError("Grid observations not available yet")

        else:
            raise ValueError("No such observation type: %s" % observation_type)



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

        if self.observation_type == "full":
            observation = Observation(pellets=state[0], viruses=state[1],
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
